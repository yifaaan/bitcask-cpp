#pragma once

#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <cstddef>
namespace cl
{
    class Db
    {
    public:
        explicit Db(const std::string& filename);
        ~Db();

        bool Open();
        bool Close();
        // 插入
        void Put(const char* key, const char* value);

    private:
        std::string                               filename_;
        std::unordered_map<uint64_t, std::string> key_dir_;
        std::unordered_map<uint64_t, std::string> open_files_;
        /// 活跃数据文件描述符
        int fd_;
        /// 活跃数据文件的id
        uint64_t          active_file_id_{};
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