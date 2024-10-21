#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "active_file.h"
#include "active_map.h"
#include "cache_metrics.h"
#include "db/option.h"
#include "record.h"
#include "record_map.h"
#include "stable_file.h"
namespace db {
class Bitcask {
public:
  Bitcask(const std::string& dir, const Option& opts);
  ~Bitcask();

  Bitcask(const Bitcask&) = delete;
  Bitcask& operator=(const Bitcask&) = delete;

  std::optional<Value> Get(const Key& key);
  std::future<void> Put(const Key& key, const Value& value);
  std::future<void> Delete(const Key& key);

private:
  struct Write;
  // 读取数据文件构建内存索引
  void RestoreFiles(std::vector<std::string>& files);
  void CreateActiveFile();

  /// 执行批量写入文件的操作
  void CommitWrites(std::vector<Write>& writes);
  /// 提交写任务
  void CommitWorker();

  /// 将活跃数据文件转换成普通数据文件
  std::shared_ptr<StableFile> Degenerate(std::shared_ptr<ActiveFile> handler);

  /// 返回当前的活跃数据文件id
  size_t NextFileId() const;

  const Option opts_;
  /// 数据库目录
  const std::string dir_;
  /// 缓存命中计算结构
  CacheMetrics metrics_;
  /// 活跃数据文件中的内存索引
  ActiveMap active_map_;
  /// 所有数据的内存索引
  RecordMap record_map_;
  /// 数据库是否运行中
  std::atomic<bool> running_;
  /// 提交批量写任务的线程，只有这一个线程负责写文件
  std::thread commit_worker_thread_;

  /// 读写mutex
  std::shared_mutex mutex_;
  /// 批量写缓存
  std::vector<Write> write_batch_;
  /// 活跃数据文件
  std::shared_ptr<ActiveFile> active_file_handler_;
  /// 数据文件
  std::vector<std::shared_ptr<StableFile>> stable_file_handlers_;
};
} // namespace db