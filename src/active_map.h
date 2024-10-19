#pragma once

#include <optional>

#include "libcuckoo/cuckoohash_map.hh"
#include "record.h"
namespace db {
class ActiveMap {
 public:
  explicit ActiveMap() = default;
  ~ActiveMap() = default;

  ActiveMap(const ActiveMap&) = delete;
  ActiveMap& operator=(const ActiveMap&) = delete;

  std::optional<Value> Get(const Key& key) const;
  void Apply(const Key& key, const Value& value);
  void Put(const Key& key, const Value& value);
  void Remove(const Key& key);
  void Clear();

 private:
  libcuckoo::cuckoohash_map<Key, Value> map_;
};
}  // namespace db