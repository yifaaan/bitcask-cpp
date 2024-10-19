#include "stable_file.h"

#include <cstdint>
#include <memory>
#include <mutex>

#include "easylogging++.h"
#include "file.h"
#include "hint.h"
#include "record.h"

namespace db {
StableFile::StableFile(std::shared_ptr<FILE> file, CacheMetrics& metrics)
    : file_{file}, metrics{metrics}, cache_{128} {}

std::shared_ptr<StableFile> StableFile::Restore(size_t file_id,
                                                std::string_view filename,
                                                CacheMetrics& metrics,
                                                RecordMap& record_map) {
  auto callback = [&record_map, file_id](const Key& key, const Value& value,
                                         const RecordInfo& info) {
    record_map.Apply(key, HintRecord{
                              .file_id_ = file_id,
                              .size_ = static_cast<uint32_t>(value.size()),
                              .offset_ = info.value_offset_,
                          });
  };
  LOG(INFO) << "restore stable file from" << filename;
  auto file = OpenFile(filename);
  ReadAllRecords(file.get(), callback);
  return std::make_shared<StableFile>(file, metrics);
}

Value StableFile::Read(const Key& key, uint32_t offset, uint32_t size) {
  auto lock = std::lock_guard{mutex_};
  // 缓存命中，就不用去读文件了
  if (cache_.contains(key)) {
    metrics.Hit();
    return cache_.lookup(key);
  }
  // 标记缓存未命中
  metrics.Miss();
  // 从文件读记录
  auto value = ReadFile(file_.get(), offset, size);
  // 更新缓存
  cache_.emplace(key, value);
  return value;
}
}  // namespace db