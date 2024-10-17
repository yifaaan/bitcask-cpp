#include "exception/io_exception.h"
#include <cstddef>
#include <cstdio>
#include <dirent.h>
#include <memory>
#include <sys/stat.h>

namespace db
{
    inline FILE* fopen_safe(const char* filename, const char* mode)
    {
        FILE* fp;
        if ((fp = fopen(filename, mode)) == nullptr)
        {
            throw_io_exception("open file to read failed");
        }
        return fp;
    }

    // 读取size个对象
    inline void fread_safe(void* ptr, size_t size, FILE* stream)
    {
        if (size == 0) return;
        if (!fread(ptr, size, 1, stream) && !feof(stream))
        {
            throw_io_exception("read file failed");
        }
    }

    /// 写入size个对象
    inline void fwrite_safe(const void* buf, size_t size, FILE* fp)
    {
        if (size == 0) return;
        if (!fwrite(buf, size, 1, fp))
        {
            throw_io_exception("read file failed");
        }
    }

    /// 返回当前文件偏移
    inline long ftell_safe(FILE* fp)
    {
        long offset = ftell(fp);
        if (offset == -1)
        {
            throw_io_exception("call ftell failed");
        }
        return offset;
    }

    inline void fseek_safe(FILE* stream, long offset, int whence)
    {
        if (fseeko(stream, offset, whence) == -1)
        {
            throw_io_exception("seek to target offset failed");
        }
    }

    inline DIR* opendir_safe(const char* dirname)
    {
        DIR* dir = opendir(dirname);
        if (dir == nullptr)
        {
            throw_io_exception("call opendir failed");
        }
        return dir;
    }

    inline void closedir_safe(DIR* dir)
    {
        if (closedir(dir) == -1)
        {
            throw_io_exception("close dir failed");
        }
    }

    inline void stat_safe(const char* name, struct stat* statbuf)
    {
        if (stat(name, statbuf) == -1)
        {
            throw_io_exception("get file information failed");
        }
    }

    inline void must_eof(FILE* fp)
    {
        if (!feof(fp))
        {
            throw_io_exception("read file failed ");
        }
    }
} // namespace db