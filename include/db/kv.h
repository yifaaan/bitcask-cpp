#pragma once

#include "db/option.h"
#include "db/type.h"
#include <future>
#include <memory>
#include <optional>
namespace db {
class Db {
public:
  static Db Create(const std::string& dir, const Option& opts);
  ~Db() = default;

  std::optional<Value> Get(const Key& key);
  std::future<void> Put(const Key& key, const Value& value);
  std::future<void> Delete(const Key& key);

private:
  Db() = default;

  class Impl;
  std::shared_ptr<Impl> impl_;
};
} // namespace db