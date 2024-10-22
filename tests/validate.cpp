#include <stdio.h>
#include <stdlib.h>

#include "easylogging++.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "db/kv.h"

using namespace db;

INITIALIZE_EASYLOGGINGPP

void test_put(Db kv, const std::vector<std::string>& keys,
              const std::vector<std::string>& values) {
  for (int i = 0; i < keys.size(); i++) {
    auto& key = keys[i];
    auto& value = values[i];
    kv.Put(key, value).get();
  }
}

void test_get(Db kv, const std::vector<std::string>& keys,
              const std::vector<std::string>& values) {
  for (int i = 0; i < keys.size(); i++) {
    auto& key = keys[i];
    auto& value = values[i];
    // LOG(INFO) << "get num " << i;
    auto option = kv.Get(key);
    auto target = option.value_or("empty");
    if (value != target) {
      std::cout << "#" << i + 1 << " "
                << "key: `" << key << "` "
                << "want: `" << value << "` "
                << "get: `" << target << "`" << std::endl;
    }
  }
}

int main() {
  std::string dir = "./tmp/";
  mkdir(dir.c_str(), 0777);
  std::vector<std::string> keys;
  std::vector<std::string> values;

  Option option{1024};

  srand(time(nullptr));

  for (size_t i = 0; i < 10; i++) {
    char buf[32];
    int r = rand();
    sprintf(buf, "key-%d", 1);
    keys.emplace_back(buf);
    sprintf(buf, "value-%d", 1);
    values.emplace_back(buf);
  }

  {
    Db kv = Db::Create(dir, option);
    test_put(kv, keys, values);
  }

  {
    Db kv = Db::Create(dir, option);
    test_get(kv, keys, values);
  }

  std::filesystem::remove_all(dir);
  return 0;
}