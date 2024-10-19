#include "file.h"

#include <sys/stat.h>

#include <cstdio>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "easylogging++.h"
#include "exception/io_exception.h"

namespace db {
std::string file_path(const std::string& dir, const std::string& filename) {
  if (dir.empty() || dir.back() == '/') {
    return dir + filename;
  } else {
    return dir + '/' + filename;
  }
}

std::vector<std::string> GetAllFiles(const std::string& dir) {
  dirent* ptr;
  struct stat statbuf;
  // 打开目录
  auto dir_p = opendir_safe(dir.data());

  std::vector<std::string> files;
  while ((ptr = readdir(dir_p)) != nullptr) {
    // 掠过 . 和 .. 目录
    if (ptr->d_name[0] == '.') continue;
    // 构造出文件路径名
    std::string filename = file_path(dir, ptr->d_name);
    stat_safe(filename.data(), &statbuf);
    if (S_ISREG(statbuf.st_mode)) {
      files.emplace_back(filename);
    }
  }

  closedir_safe(dir_p);
  return files;
}

std::shared_ptr<FILE> OpenFile(const std::string_view filename) {
  LOG(INFO) << "open file" << filename;
  auto fp = fopen_safe(filename.data(), "a+b");
  return std::shared_ptr<FILE>(fp, std::fclose);
}

long WriteFile(const void* buf, size_t size, FILE* fp) {
  // 获取当前写偏移
  auto offset = ftell_safe(fp);
  if (size != 0) {
    fwrite_safe(buf, size, fp);
  }
  return offset;
}

bool ReadFile(FILE* fp, void* buf, size_t size) {
  if (size == 0) return true;
  if (feof(fp)) {
    // LOG(TRACE) << "feof";
    return false;
  }
  // LOG(TRACE) << "before read safe";
  fread_safe(buf, size, fp);
  // LOG(TRACE) << "after read safe";

  return true;
}
std::string ReadFile(FILE* fp, size_t size) {
  std::string content(size, '\0');
  fread_safe(content.data(), size, fp);
  return content;
}
std::string ReadFile(FILE* fp, uint32_t offset, size_t size) {
  std::string content(size, '\0');
  fseek_safe(fp, offset, SEEK_SET);
  fread_safe(content.data(), size, fp);
  return content;
}
}  // namespace db