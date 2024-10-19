#pragma once

#include <cerrno>
#include <cstring>
#include <stdexcept>
class io_exception : public std::runtime_error {
 public:
  explicit io_exception(const std::string& arg) : std::runtime_error{arg} {}

  explicit io_exception(const char* arg) : std::runtime_error{arg} {}

  ~io_exception() = default;
};

inline void throw_io_exception(const std::string& message) {
  char errinfo[64]{};
  errinfo[0] = ':';

  strerror_r(errno, errinfo + 1, sizeof(errinfo) - 1);
  throw io_exception{message + errinfo};
}