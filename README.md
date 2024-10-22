## bitcask-cpp
- A storage engine for learning

## Dependency:

* [CRC32](https://github.com/google/crc32c)
* [GoogleTest](https://github.com/google/googletest.git)
* [ezsyloggingpp](https://github.com/muflihun/easyloggingpp.git)
* [libcuckoo](https://github.com/efficient/libcuckoo.git)

## How to Run
```
git clone git@github.com:yifaaan/bitcask-cpp.git
cd bitcask-cpp
chmod +x dependency.sh
chmod +x third.sh
./third.sh
mkdir build && cd build
cmake ..
make
```

## Usage

```C++
#include <string>

#include "easylogging++.h"

#include "db/kv.h"

using namespace db;

INITIALIZE_EASYLOGGINGPP

int main() {
  std::string dir = "./tmp/";
  Option option { 1024 };
    
  BordKV kv = db::Create(dir, option);
  auto option = kv.Get("key");
  auto future = kv.Write("key", "value");
  auto future1 = kv.Delete("key");
}
```