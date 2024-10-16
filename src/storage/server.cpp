#include "server.h"
#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

namespace cl
{
    namespace fs = std::filesystem;

    Db::Db(const std::string& filename) : filename_{filename}
    {}

    Db::~Db()
    {}

    bool Db::Open()
    {
        if (fd_ = open("a.txt", O_RDWR); fd_ == -1)
        {
            std::cerr << "can not open the file\n";
            return false;
        }
        std::cout << "successful to open the file\n";
        return true;
    }
} // namespace cl