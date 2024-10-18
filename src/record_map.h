#pragma once

#include <optional>

#include "libcuckoo/cuckoohash_map.hh"
#include "db/type.h"
#include "hint.h"

namespace db
{
    class RecordMap
    {
    public:
        explicit RecordMap() = default;
        ~RecordMap()         = default;

        RecordMap(const RecordMap&)            = delete;
        RecordMap& operator=(const RecordMap&) = delete;

        /// 返回HintRecord
        std::optional<HintRecord> Get(const Key& key) const;
        ///
        void Apply(const Key& key, const HintRecord& value);
        ///
        void Put(const Key& key, const HintRecord& value);

        ///
        void Remove(const Key& key);

        ///
        void Clear();

    private:
        libcuckoo::cuckoohash_map<Key, HintRecord> map_;
    };
} // namespace db