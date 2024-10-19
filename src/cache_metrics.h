#pragma once

#include "easylogging++.h"
#include <atomic>
#include <cstdint>
namespace db
{
    /// 统计缓存命中率的类
    class CacheMetrics
    {
      public:
        CacheMetrics() = default;

        CacheMetrics(const CacheMetrics&)            = delete;
        CacheMetrics& operator=(const CacheMetrics&) = delete;

        ~CacheMetrics()
        {
            uint64_t hit   = hit_;
            uint64_t miss  = miss_;
            uint64_t total = hit + miss;

            if (total == 0)
                total = 1;
            LOG(INFO) << "cache total call: " << total;
            LOG(INFO) << "cache miss call: " << miss;
            LOG(INFO) << "cache hit/total: " << ((double)hit) / total;
        }

        void Hit()
        {
            hit_.fetch_add(1, std::memory_order_relaxed);
        }

        void Miss()
        {
            miss_.fetch_add(1, std::memory_order_relaxed);
        }

      private:
        std::atomic<uint64_t> hit_{};
        std::atomic<uint64_t> miss_{};
    };
} // namespace db