#pragma once

#include "crc32c/crc32c.h"

#include <cstdint>
#include <string_view>
namespace db
{

    template <typename T>
    inline uint32_t crc32(uint32_t crc, T* buf, size_t len)
    {
        const uint8_t* buffer = reinterpret_cast<uint8_t*>(buf);
        return crc32c::Extend(crc, buffer, len);
    }

    template <>
    inline uint32_t crc32(uint32_t crc, const uint8_t* buf, size_t len)
    {
        return crc32c::Extend(crc, buf, len);
    }

    inline uint32_t crc32(uint32_t crc, std::string_view s)
    {
        return crc32c::Extend(
            crc, reinterpret_cast<const uint8_t*>(s.data()), s.size());
    }

} // namespace db