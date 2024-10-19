#pragma once
#include <cstddef>
#include <list>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "record.h"
namespace db {
/// LRU缓存
template <typename K, typename V>
class Cache {
  using KeyValuePair = std::pair<K, V>;
  using kv_iterator = std::list<KeyValuePair>::iterator;

 public:
  /// 缓存的最大容量
  explicit Cache(size_t max_size) : max_size_{max_size} {}

  /// 插入新缓存项
  void emplace(const K &key, const V &value) {
    /// 插入到队列头，表示最近使用过
    cache_items_list_.push_front(KeyValuePair{key, value});
    /// 存在旧的就删除
    if (auto it = cache_items_map_.find(key); it != cache_items_map_.end()) {
      cache_items_list_.erase(it->second);
      cache_items_map_.erase(it);
    }
    /// 更新位置
    cache_items_map_[key] = cache_items_list_.begin();

    /// 容量溢出，删除最久未使用的项
    if (cache_items_map_.size() > max_size_) {
      auto last = cache_items_list_.end();
      last--;
      cache_items_map_.erase(last->first);
      cache_items_list_.pop_back();
    }
  }

  /// 查找缓存项
  const Value &lookup(const K &key) {
    if (auto it = cache_items_map_.find(key); it != cache_items_map_.end()) {
      // 将list中的该项移动到list的开头
      cache_items_list_.splice(cache_items_list_.begin(), cache_items_list_,
                               it->second);
      return it->second->second;
    } else {
      throw std::range_error("There is no such key in cache");
    }
  }

  bool contains(const K &key) const { return cache_items_map_.contains(key); }

  size_t size() const { return cache_items_map_.size(); }

 private:
  std::list<KeyValuePair> cache_items_list_;
  /// key映射到list中的位置
  std::unordered_map<Key, kv_iterator> cache_items_map_;
  size_t max_size_;
};
}  // namespace db