
#include "bitcask.h"
#include "active_file.h"
#include "db/option.h"
#include "easylogging++.h"
#include "file.h"
#include "hint.h"
#include "record.h"
#include "stable_file.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace db {
constexpr std::string db_file_suffix(".kvr");

/// 根据目录名和id生成数据文件名称
static std::string GenerateFileName(const std::string& dir, size_t id = 0) {
  char buf[12]{};
  sprintf(buf, "%010zu", id);
  return file_path(dir, buf + db_file_suffix);
}

static void SortedStableFileByTimestamp(std::vector<std::string>& files) {
  std::ranges::sort(files, std::less<std::string>{});
}

/// 过滤出数据文件
static std::vector<std::string>
FilterDBFiles(const std::vector<std::string>& files) {
  std::vector<std::string> target_files;
  for (const auto& name : files) {
    if (name.ends_with(db_file_suffix)) {
      target_files.push_back(name);
    }
  }
  return target_files;
}

struct Bitcask::Write {
  const Key key_;
  const Value value_;
  std::promise<void> promise_;

  Write(Write&& rhs)
      : key_{std::move(rhs.key_)}, value_{std::move(rhs.value_)},
        promise_{std::move(rhs.promise_)} {}

  Write(const Key& key, const Value& value) : key_{key}, value_{value} {}
};

Bitcask::Bitcask(const std::string& dir, const Option& opts)
    : opts_{opts}, dir_{dir}, running_{true} {
  LOG(INFO) << "open db at " << dir;

  /// Clear All Temporary File(dir)
  auto files = FilterDBFiles(GetAllFiles(dir));
  if (files.empty()) {
    // 当前目录是空的，创建一个id=0的活跃数据文件
    LOG(DEBUG) << "Create Active File";
    CreateActiveFile();
  } else {
    // 存在数据文件，需要加载
    RestoreFiles(files);
  }
  commit_worker_thread_ = std::thread(std::bind(&Bitcask::CommitWorker, this));
}

Bitcask::~Bitcask() {
  LOG(INFO) << "close db";
  running_ = false;
  commit_worker_thread_.join();
}

void Bitcask::CreateActiveFile() {
  auto active_file = GenerateFileName(dir_);
  LOG(INFO) << "current dir is empty, try create active file " << active_file;
  active_file_handler_ = ActiveFile::Create(active_file);
}

void Bitcask::RestoreFiles(std::vector<std::string>& files) {
  SortedStableFileByTimestamp(files);
  // 活跃数据文件的编号最大
  auto active_file = files.back();
  files.pop_back();

  for (const auto& filename : files) {
    LOG(INFO) << "find stable file " << filename;
    auto handle =
        StableFile::Restore(NextFileId(), filename, metrics_, record_map_);
    stable_file_handlers_.emplace_back(handle);
  }

  LOG(INFO) << "find active file " << active_file;
  // 加载活跃数据文件内存索引
  active_file_handler_ =
      ActiveFile::Restore(NextFileId(), active_file, record_map_, active_map_);
}

std::optional<Value> Bitcask::Get(const Key& key) {
  // 内存索引查询
  auto option = record_map_.Get(key);
  if (!option) {
    return std::nullopt;
  }

  auto& record = *option;
  // 读锁，因为写文件的线程会修改stable_file_handlers_
  auto lock = std::shared_lock{mutex_};
  // 如果在活跃数据文件中
  if (record.file_id_ == NextFileId()) {
    lock.unlock();
    auto value = active_map_.Get(key);
    if (value)
      return value;
    throw std::runtime_error{"error, no such key"};
  } else {
    auto& stable_file_handler = stable_file_handlers_[record.file_id_];
    lock.unlock();
    return stable_file_handler->Read(key, record.offset_, record.size_);
  }
}

std::future<void> Bitcask::Put(const Key& key, const Value& value) {
  if (!running_) {
    throw std::runtime_error{"call Bitcask:: on stopped db"};
  }

  auto lock = std::unique_lock{mutex_};
  write_batch_.emplace_back(key, value);
  return write_batch_.back().promise_.get_future();
}

std::future<void> Bitcask::Delete(const Key& key) {
  if (!running_) {
    throw std::runtime_error{"call Bitcask:: on stopped db"};
  }

  auto lock = std::unique_lock{mutex_};

  Value target_value;
  write_batch_.emplace_back(key, target_value);
  return write_batch_.back().promise_.get_future();
}

size_t Bitcask::NextFileId() const { return stable_file_handlers_.size(); }

std::shared_ptr<StableFile>
Bitcask::Degenerate(std::shared_ptr<ActiveFile> handler) {
  return std::make_shared<StableFile>(handler->Rotate(), metrics_);
}

void Bitcask::CommitWrites(std::vector<Write>& writes) {
  // 从活跃变成stable的文件列表
  std::vector<std::shared_ptr<StableFile>> stable_handlers;
  std::vector<HintRecord> wait_commit_records;

  auto active_handler = active_file_handler_;

  uint32_t value_offset{};
  // 活跃数据文件第一条记录在writes数组的索引
  size_t first_active_record{};
  size_t next_file_id = NextFileId();
  const size_t commit_group_size = writes.size();

  for (size_t i = 0; i < commit_group_size; i++) {
    auto& [key, value, promise] = writes[i];
    value_offset = active_handler->Wirte(key, value);
    auto record = HintRecord{next_file_id, static_cast<uint32_t>(value.size()),
                             value_offset};
    wait_commit_records.emplace_back(record);
    // 如果还可以向活跃数据文件写入一条记录
    if (value_offset + value.size() < opts_.log_file_size_)
      continue;
    // 否则需要创建新的活跃数据文件
    active_handler->Sync();
    // 当前活跃文件变成stable
    stable_handlers.emplace_back(Degenerate(active_handler));
    auto new_file_name = GenerateFileName(dir_, ++next_file_id);
    active_handler = ActiveFile::Create(new_file_name);
    first_active_record = i + 1;
  }
  active_handler->Sync();

  {
    auto lock = std::unique_lock{mutex_};
    // 更新stable数据文件列表
    stable_file_handlers_.insert(stable_file_handlers_.end(),
                                 stable_handlers.begin(),
                                 stable_handlers.end());
    // 更新当前活跃数据文件
    active_file_handler_ = active_handler;
  }

  // 如果产生了新的活跃数据文件，就需要清空之前的旧索引
  if (!stable_handlers.empty()) {
    active_map_.Clear();
  }

  for (size_t i = 0; i < commit_group_size; i++) {
    auto& [key, value, promise] = writes[i];
    auto& record = wait_commit_records[i];
    // 如果属于活跃数据文件中的数据，就插入
    if (first_active_record <= i) {
      active_map_.Apply(key, value);
    }
    // 通知写入完成
    promise.set_value();
    // 插入内存索引
    record_map_.Apply(key, record);
  }
}

void Bitcask::CommitWorker() {

  std::vector<Write> writes;
  while (true) {
    {
      auto lock = std::unique_lock{mutex_};
      std::swap(writes, write_batch_);
    }
    if (!writes.empty()) {
      // 批量写入文件
      CommitWrites(writes);
      writes.clear();
    }

    if (!running_) {
      break;
    }
    std::this_thread::yield();
  }
}
} // namespace db