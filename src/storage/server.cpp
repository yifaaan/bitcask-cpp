#include "server.h"
#include "../utils/common.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <mutex>
#include <optional>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <charconv>
/*
        +-------+--------+--------+----------+-----+-------+--------+
        | crc32 | tstamp | key_sz | value_sz | key | value | offset |
        +-------+--------+--------+----------+-----+-------+--------+
*/

namespace cl
{
    constexpr auto BUFSIZE = 2048;

    LogRecord::LogRecord(int fd, uint64_t offset, uint64_t size) :
        fd_{fd}, offset_{offset}, size_{size}
    {}

    Db::Db(const std::string& filename) : filename_{filename}
    {}

    Db::~Db()
    {
        close(fd_);
    }

    bool Db::Open()
    {
        if (fd_ = open(filename_.c_str(), O_RDWR | O_CREAT); fd_ == -1)
        {
            std::cerr << "can not open the file\n";
            return false;
        }
        std::cout << "successful to open the file\n";
        return true;
    }

    bool Db::Put(const std::string& key, const std::string& value)
    {
        if (fd_ == -1)
        {
            std::cerr << "file don't open.\n";
            return false;
        }

        std::unique_lock lock(mutex_);
        bool             is_new    = false;
        uint32_t         uint32_sz = sizeof(uint32_t);
        uint32_t         key_sz    = key.size();
        uint32_t         value_sz  = value.size();
        uint32_t         tuple_sz  = 4 * uint32_sz + key_sz + value_sz;

        char buf[BUFSIZE]{};
        // 计算crc
        uint32_t tstamp = timestamp();
        auto     res =
            std::to_chars(std::begin(buf), std::begin(buf) + uint32_sz, tstamp);

        res = std::to_chars(res.ptr, res.ptr + uint32_sz, key_sz);

        res = std::to_chars(res.ptr, res.ptr + uint32_sz, key_sz);

        auto ptr = std::copy(std::begin(key), std::end(key), res.ptr);
        ptr      = std::copy(std::begin(key), std::end(key), ptr);

        auto len = std::distance(std::begin(buf), ptr);
        /// TODO
        // uint32_t checksum = crc32_checksum(buf, len);
        uint32_t checksum = 20;
        char     sup_buf[BUFSIZE]{};
        res = std::to_chars(
            std::begin(sup_buf), std::begin(sup_buf) + uint32_sz, checksum);
        ptr = std::copy_n(std::begin(buf), len, res.ptr);

        len += uint32_sz;
        std::cout << "len : " << len << '\n';
        // 别忘了设置写指针
        lseek(fd_, offset_, SEEK_SET);
        int ret = write(fd_, sup_buf, len);
        if (ret < 0)
        {
            std::cerr << "write failed\n";
            return false;
        }

        auto record = LogRecord{fd_, offset_, static_cast<uint64_t>(len)};
        offset_ += len;

        key_dir_[key] = record;
        return true;
    }

    std::optional<std::string> Db::Get(const std::string& key)
    {
        if (fd_ == -1)
        {
            std::cerr << "file don't open\n";
            return std::nullopt;
        }

        if (key_dir_.find(key) == key_dir_.end())
        {
            std::cerr << "db don't exist the key = " << key << '\n';
            return std::nullopt;
        }
        return "haha";
    }
} // namespace cl