#include "record_map.h"
#include "hint.h"
#include <optional>
#include <stdexcept>

namespace db
{
    std::optional<HintRecord> RecordMap::Get(const Key& key) const
    {
        try
        {
            return map_.find(key);
        }
        catch (std::out_of_range& e)
        {
            return std::nullopt;
        }
    }

    void RecordMap::Apply(const Key& key, const HintRecord& value)
    {
        if (value.size_ == 0)
        {
            Remove(key);
        }
        else
        {
            Put(key, value);
        }
    }

    void RecordMap::Put(const Key& key, const HintRecord& value)
    {
        map_.insert_or_assign(key, value);
    }

    void RecordMap::Remove(const Key& key)
    {
        map_.erase(key);
    }

    void RecordMap::Clear()
    {
        map_.clear();
    }

} // namespace db