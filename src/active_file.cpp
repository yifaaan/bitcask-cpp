
#include "active_file.h"
#include "easylogging++.h"
#include "file.h"
#include "hint.h"
#include "record.h"
#include <cstdint>
#include <cstdio>
#include <memory>
#include <unistd.h>
#include <utility>

namespace db
{
    ActiveFile::ActiveFile(std::shared_ptr<FILE> file) : file_{file}
    {
        fseek_safe(file_.get(), 0, SEEK_END);
    }

    uint32_t ActiveFile::Wirte(const Key& key, const Value& value)
    {
        auto record_info = WriteRecord(key, value, file_.get());
        return record_info.value_offset_;
    }

    void ActiveFile::Sync()
    {
        auto fp = file_.get();
        // 写入内核缓冲区
        fflush(fp);
        // 写入磁盘
        fsync(fileno(fp));
    }

    std::shared_ptr<ActiveFile> ActiveFile::Create(std::string_view filename)
    {
        LOG(INFO) << "create a active file" << filename;

        return std::make_shared<ActiveFile>(OpenFile(filename));
    }

    std::shared_ptr<ActiveFile> ActiveFile::Restore(size_t file_id, const std::string_view filename,
                                                    RecordMap& record_map, ActiveMap& map)
    {
        auto callback = [&record_map, &map, file_id](const Key& key, const Value& value, const RecordInfo& info)
        {
            map.Apply(key, value);
            record_map.Apply(key, HintRecord{.file_id_ = file_id,
                                             .size_    = static_cast<uint32_t>(value.size()),
                                             .offset_  = info.value_offset_});
        };

        LOG(INFO) << "restore active file form " << filename;

        auto f = OpenFile(filename);
        ReadAllRecords(f.get(), callback);

        return std::make_shared<ActiveFile>(f);
    }

    std::shared_ptr<FILE> ActiveFile::Rotate()
    {
        std::shared_ptr<FILE> f;
        std::swap(f, file_);
        return f;
    }
} // namespace db