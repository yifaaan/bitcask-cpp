#include <dirent.h>
#include <sys/stat.h>

#include <cstddef>
#include <cstdio>
#include <memory>
#include <string_view>
#include <vector>

#include "exception/io_exception.h"

namespace db {
inline FILE* fopen_safe(const char* filename, const char* mode) {
  FILE* fp;
  if ((fp = fopen(filename, mode)) == nullptr) {
    throw_io_exception("open file to read failed");
  }
  return fp;
}

/// 读size个字节
inline void fread_safe(void* ptr, size_t size, FILE* stream) {
  if (size == 0)
    return;
  if (!fread(ptr, size, 1, stream) && !feof(stream)) {
    throw_io_exception("read file failed");
  }
}

/// 写入size个字节
inline void fwrite_safe(const void* buf, size_t size, FILE* fp) {
  if (size == 0)
    return;
  if (!fwrite(buf, size, 1, fp)) {
    throw_io_exception("write file failed");
  }
}

/// 返回当前文件偏移
inline long ftell_safe(FILE* fp) {
  long offset = ftell(fp);
  if (offset == -1) {
    throw_io_exception("call ftell failed");
  }
  return offset;
}

inline void fseek_safe(FILE* stream, long offset, int whence) {
  if (fseeko(stream, offset, whence) == -1) {
    throw_io_exception("seek to target offset failed");
  }
}

inline DIR* opendir_safe(const char* dirname) {
  DIR* dir = opendir(dirname);
  if (dir == nullptr) {
    throw_io_exception("call opendir failed");
  }
  return dir;
}

inline void closedir_safe(DIR* dir) {
  if (closedir(dir) == -1) {
    throw_io_exception("close dir failed");
  }
}

inline void stat_safe(const char* name, struct stat* statbuf) {
  if (stat(name, statbuf) == -1) {
    throw_io_exception("get file information failed");
  }
}

inline void must_eof(FILE* fp) {
  if (!feof(fp)) {
    throw_io_exception("read file failed ");
  }
}

/// 根据目录和文件名构造出文件的完整路径
std::string file_path(const std::string& dir, const std::string& filename);

/// 返回该目录下的所有文件路径
std::vector<std::string> GetAllFiles(const std::string& dir);

std::shared_ptr<FILE> OpenFile(const std::string_view filename);

/// 写入每个元素大小为size字节的buf，返回写入之前的文件偏移
long WriteFile(const void* buf, size_t size, FILE* fp);

// 从文件读取size个字节
bool ReadFile(FILE* fp, void* buf, size_t size);
// 从文件读取size个字节
std::string ReadFile(FILE* fp, size_t size);
// 从文件offset处读取size个字节
std::string ReadFile(FILE* fp, uint32_t offset, size_t size);

} // namespace db