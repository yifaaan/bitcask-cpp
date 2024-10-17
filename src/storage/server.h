#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <cstddef>
namespace cl
{

    /*
        +-------+--------+--------+----------+-----+-------+--------+
        | crc32 | tstamp | key_sz | value_sz | key | value | offset |
        +-------+--------+--------+----------+-----+-------+--------+
    */

    /// 数据文件的一条记录
    class LogRecord
    {
    public:
        LogRecord() = default;
        LogRecord(int fd, uint64_t offset, uint64_t size);

        /// 该记录所处的文件
        int fd_{};
        /// 文件内的偏移位置
        uint64_t offset_{};
        /// 该记录的大小
        uint64_t size_{};
    };

    class Db
    {
    public:
        explicit Db(const std::string& filename);
        ~Db();

        bool Open();
        bool Close();
        /// 插入
        bool Put(const std::string& key, const std::string& value);
        /// 获取
        std::optional<std::string> Get(const std::string& key);

        bool Flush();
        bool Sync();
        bool Close();

    private:
        std::string filename_;
        /// 内存索引表
        std::unordered_map<std::string, LogRecord> key_dir_;
        std::unordered_map<uint64_t, std::string>  open_files_;
        /// 活跃数据文件描述符
        int         fd_;
        std::string data_;
        /// 活跃数据文件的id
        uint64_t          active_file_id_{};
        uint64_t          offset_{};
        size_t            size_{};
        bool              is_opened_{};
        std::shared_mutex mutex_;

        static constexpr auto TOMBSTONE           = "DB_TOMBSTONE_VALUE";
        static constexpr auto DATA_FILE_EXTENTION = ".data";
        static constexpr auto HINT_FILE_EXTENTION = ".hint";
        static constexpr auto TEMP_FILE_EXTENTION = ".tmp";
        static constexpr auto LOCK_FILE           = ".lock";
    };
} // namespace cl