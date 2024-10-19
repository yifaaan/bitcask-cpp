#pragma once

#include <cstddef>
#include <cstdint>
namespace db {
struct HintRecord {
  size_t file_id_;
  uint32_t size_;
  uint32_t offset_;
};
}  // namespace db