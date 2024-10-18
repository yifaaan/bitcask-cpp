#include "record.h"
#include "crc32.h"
#include "file.h"
#include "easylogging++.h"

#include <cstdint>
#include <cstdio>

namespace db
{

    struct RecordHeader
    {
        uint32_t crc32_;
        uint32_t key_size_;
        uint32_t value_size_;
    };

    /// 返回一条记录的crc
    static uint32_t
    GetCrc32(RecordHeader& header, const Key& key, const Value& value)
    {
        uint32_t crc{};
        crc = crc32(crc, &header.key_size_, sizeof(header.key_size_));
        crc = crc32(crc, &header.value_size_, sizeof(header.value_size_));
        crc = crc32(crc, key);
        crc = crc32(crc, value);
        return crc;
    }

    /// 设置header,返回一条记录的总大小
    static size_t
    FillHeader(RecordHeader& header, const Key& key, const Value& value)
    {
        header.key_size_   = static_cast<uint32_t>(key.size());
        header.value_size_ = static_cast<uint32_t>(value.size());
        header.crc32_      = GetCrc32(header, key, value);
        return sizeof(header) + key.size() + value.size();
    }

    RecordInfo WriteRecord(const Key& key, const Value& value, FILE* fp)
    {
        RecordHeader header;
        RecordInfo   info;

        info.size_ = FillHeader(header, key, value);

        // 向活跃数据文件
        // 写入header
        WriteFile(&header, sizeof(header), fp);
        // 写入key
        info.key_offset_ = WriteFile(key.data(), key.size(), fp);
        // 写入value
        info.value_offset_ = WriteFile(value.data(), value.size(), fp);

        LOG(TRACE) << "write record crc32: " << header.crc32_ << " key: `"
                   << key << "` s: " << header.key_size_ << " value: `" << value
                   << "` s: " << header.value_size_;
        return info;
    }

    bool ReadRecord(RecordInfo& info, Key& key, Value& value, FILE* fp)
    {
        RecordHeader header;
        key.clear();
        value.clear();
        info.key_offset_ = info.value_offset_ = info.size_ = 0;

        LOG(TRACE) << "before read header";
        // 读header
        if (!ReadFile(fp, &header, sizeof(header)))
        {
            return false;
        }
        LOG(TRACE) << "crc: " << header.crc32_ << " ks: " << header.key_size_
                   << " vs: " << header.value_size_;
        // 设置记录的大小
        info.size_ = sizeof(header) + header.key_size_ + header.value_size_;
        info.key_offset_ = ftell_safe(fp);
        LOG(TRACE) << "key_offset: " << info.key_offset_;
        key                = ReadFile(fp, header.key_size_);
        info.value_offset_ = ftell_safe(fp);
        LOG(TRACE) << "value_offset: " << info.value_offset_;
        value = ReadFile(fp, header.value_size_);

        LOG(TRACE) << "key: " << key << " value: " << value;
        // 根据读出的数据计算CRC，与存储的CRC对比
        if (GetCrc32(header, key, value) != header.crc32_)
        {
            LOG(TRACE) << "crc32 wrong, want: " << header.crc32_
                       << " but caculate: " << GetCrc32(header, key, value);
            throw io_exception("checksum failed");
        }
        LOG(TRACE) << "read record crc32: " << header.crc32_ << " key: `" << key
                   << "` s: " << header.key_size_ << " value: `" << value
                   << "` s: " << header.value_size_;
        return true;
    }

    void ReadAllRecords(FILE* fp, ReadRecordCallback callback)
    {
        RecordInfo info;
        Key        key;
        Value      value;
        uint32_t   cnt = 0;

        fseek_safe(fp, 0, SEEK_SET);
        while (ReadRecord(info, key, value, fp))
        {
            callback(key, value, info);
            cnt++;
        }
        must_eof(fp);

        LOG(INFO) << "read " << cnt << " record from a file";
    }
} // namespace db
