#pragma once

#include <cstddef>
#include <functional>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <unordered_map>
#include <vector>
namespace db {

/// 有读写锁的map
template <typename K, typename V>
class locked_map {
 public:
  void insert_or_assign(const K& key, const V& value) {
    auto lock = std::lock_guard{mutex_};
    map_.insert_or_assign(key, value);
  }

  std::optional<V> get(const K& key) const {
    auto lock = std::shared_lock{mutex_};
    if (auto it = map_.find(); it != map_.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  bool erase(const K& key) {
    auto lock = std::lock_guard{mutex_};
    return map_.erase(key) != map_.end();
  }

  void clear() {
    auto lock = std::lock_guard{mutex_};
    map_.clear();
  }

 private:
  std::shared_mutex mutex_;
  std::unordered_map<K, V> map_;
};

/// 内部是一组map
template <typename K, typename V>
class shareding_map {
 public:
  /// 大小必须是二次幂
  explicit shareding_map(size_t num_shared = 128)
      : mask_{num_shared - 1}, shareds_{num_shared} {
    if ((num_shared & mask_) != 0) {
      throw std::runtime_error{"num_shared must be a power of two"};
    }
  }

  void insert_or_assgn(const K& key, const V& value) {
    get_shared(key).insert_or_assign(key, value);
  }

  std::optional<V> get(const K& key) const { return get_shared(key).get(key); }

  bool erase(const K& key) { return get_shared(key).erase(key); }

  void clear() {
    /// TODO
  }

 private:
  /// 获取存储该key的map
  locked_map<K, V>& get_shared(const K& key) {
    auto h = hash_fn_(key);
    return shareds_[h & mask_];
  }

  const locked_map<K, V>& get_shared(const K& key) const {
    auto h = hash_fn_(key);
    return shareds_[h & mask_];
  }

  const size_t mask_;
  std::vector<locked_map<K, V>> shareds_;
  std::hash<K> hash_fn_;
};
}  // namespace db