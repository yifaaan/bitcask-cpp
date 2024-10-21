#pragma once

#include <cstddef>
#include <cstdint>
namespace db {
struct HintRecord {
  /// 该记录对应的文件id
  size_t file_id_;
  /// value的大小
  uint32_t size_;
  /// value在文件中的偏移位置
  uint32_t offset_;
};
} // namespace db