#pragma once

#include <cstdint>
#include <functional>

#include "db/type.h"

namespace db {
struct RecordInfo {
  uint32_t key_offset_;
  uint32_t value_offset_;
  /// 一条记录的总长度
  uint32_t size_;
};

using ReadRecordCallback =
    std::function<void(const Key&, const Value&, const RecordInfo&)>;

/// 读取文件fp中的所有记录，每读一条，执行一次callback
void ReadAllRecords(FILE* fp, ReadRecordCallback callback);
/// 向文件fp写入一条记录
RecordInfo WriteRecord(const Key& key, const Value& value, FILE* fp);
/// 从文件fp读取一条记录
bool ReadRecord(RecordInfo& info, Key& key, Value& value, FILE* fp);
}  // namespace db