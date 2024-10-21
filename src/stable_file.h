#pragma once
#include <cstdint>
#include <memory>
#include <mutex>
#include <string_view>

#include "cache.h"
#include "cache_metrics.h"
#include "db/type.h"
#include "record_map.h"

namespace db {
/// 数据文件
class StableFile {
public:
  explicit StableFile(std::shared_ptr<FILE> file, CacheMetrics& metrics);
  ~StableFile() = default;

  StableFile(const StableFile&) = delete;
  StableFile& operator=(const StableFile&) = delete;

  /// 从数据文件加载HintRecord索引RecordMap
  static std::shared_ptr<StableFile> Restore(size_t file_id,
                                             std::string_view filename,
                                             CacheMetrics& metrics,
                                             RecordMap& record_map);

  /// 从指定偏移处读对应的value
  Value Read(const Key& key, uint32_t offset, uint32_t size);

private:
  mutable std::mutex mutex_;
  std::shared_ptr<FILE> file_;
  /// 缓存命中统计
  CacheMetrics& metrics;
  /// 每个数据文件对应一个LRU缓存
  Cache<db::Key, db::Value> cache_;
};
} // namespace db