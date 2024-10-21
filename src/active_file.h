#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string_view>

#include "active_map.h"
#include "record.h"
#include "record_map.h"
namespace db {

/// 活跃数据文件
class ActiveFile {
public:
  explicit ActiveFile(std::shared_ptr<FILE> file);
  ~ActiveFile() = default;

  ActiveFile(const ActiveFile&) = delete;
  ActiveFile& operator=(const ActiveFile&) = delete;

  /// 创建一个活跃数据文件对象
  static std::shared_ptr<ActiveFile> Create(std::string_view filename);
  /// 加载活跃数据文件，并构建活跃数据内存索引
  static std::shared_ptr<ActiveFile> Restore(size_t file_id,
                                             const std::string_view filename,
                                             RecordMap& record_map,
                                             ActiveMap& map);

  /// 追加写入一条记录,返回其value的文件偏移
  uint32_t Wirte(const Key& key, const Value& value);
  /// 获得活跃数据文件对应的文件指针
  std::shared_ptr<FILE> Rotate();
  void Sync();

private:
  std::shared_ptr<FILE> file_;
};
} // namespace db