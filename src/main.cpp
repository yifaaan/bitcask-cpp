#include "server.h"
#include <string>
#include <iostream>

#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(dbserver, BasicAssertions)
{
    auto db = cl::Db("hello.db");
    db.Open();
    const char* key   = "Liuyifan";
    const char* value = "2024-hello-world";
    bool        ret   = db.Put(key, value);
    // std::cout << ret << std::endl;
    // Expect two strings not to be equal.
    // EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(ret, true);
}

// int main()
// {
//     auto db = cl::Db("hello.db");
//     db.Open();
//     const char* key   = "Liuyifan";
//     const char* value = "2024-hello-world";
//     bool        ret   = db.Put(key, value);
//     std::cout << ret << std::endl;
//     // REQUIRE(ret == true);
// }