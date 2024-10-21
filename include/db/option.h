#pragma once

#include <cstddef>
namespace db {
struct Option {
  size_t log_file_size_;
  size_t cache_size_;
};
} // namespace db