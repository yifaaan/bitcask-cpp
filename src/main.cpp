#include "record.h"
#include "server.h"
#include "file.h"
#include "gtest/gtest.h"
#include "crc32.h"
#include "fcntl.h"
#include "easylogging++.h"

#include <cstdio>
#include <string>
#include <iostream>
INITIALIZE_EASYLOGGINGPP
// Demonstrate some basic assertions.
TEST(dbserver, BasicAssertions)
{
    using db::Key;
    using db::Value;

    Key   key1   = "Liuyifan";
    Value value1 = "2024-hello-world";

    auto f = db::OpenFile("0.data");

    auto info1 = db::WriteRecord(key1, value1, f.get());
    fflush(f.get());
    auto f2 = db::OpenFile("0.data");

    db::RecordInfo info2;
    Key            key2;
    Value          value2;

    auto r = db::ReadRecord(info2, key2, value2, f2.get());

    LOG(TRACE) << r;
    // std::cout << ret << std::endl;
    // Expect two strings not to be equal.
    // EXPECT_STRNE("hello", "world");
    // Expect equality.
    // uint8_t a[3] = {1, 2, 3};
    // std::cout << crc32c::Extend(0, a, 3);
    // LOG(INFO) << "Log using default file";
    // EXPECT_EQ(info1.size_, info2.size_);
}

// int main()
// {
// auto db = cl::Db("hello.db");
// db.Open();
// const char* key   = "Liuyifan";
// const char* value = "2024-hello-world";
// bool        ret   = db.Put(key, value);
// std::cout << ret << std::endl;

// REQUIRE(ret == true);
// }