#include "server.h"
#include "common.h"
#include <string>
#include <iostream>

int main()
{
    auto db = cl::Db("hello.db");
    db.Open();
    const char* key   = "Liuyifan";
    const char* value = "2024-hello-world";
    bool        ret   = db.Put(key, value);
    std::cout << ret << std::endl;
    // REQUIRE(ret == true);
}