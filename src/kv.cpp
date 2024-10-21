#include "db/kv.h"
#include "bitcask.h"
#include <memory>
#include <optional>

namespace db {
class Db::Impl : public Bitcask {

public:
  using Bitcask::Bitcask;

  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
};

Db Db::Create(const std::string& dir, const Option& opts) {
  Db db;
  db.impl_ = std::make_shared<Db::Impl>(dir, opts);
  return db;
}

std::optional<Value> Db::Get(const Key& key) { return impl_->Get(key); }
std::future<void> Db::Put(const Key& key, const Value& value) {
  return impl_->Put(key, value);
}

std::future<void> Db::Delete(const Key& key) { return impl_->Delete(key); }
} // namespace db