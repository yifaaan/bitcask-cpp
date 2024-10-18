cmake_minimum_required(VERSION 3.24)

project(bitcask VERSION 0.1 DESCRIPTION "a key-value storage engine." LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 可执行文件
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
# 静态库
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
# 动态库
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)



#添加一个包含 CMakeLists.txt 的子目录
add_subdirectory(src)
#添加一个包含 CMakeLists.txt 的子目录
# add_subdirectory(tests)


# third_party
# 添加 googletest 子目录
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/googletest)

# 添加easyloggingpp子目录
# add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/easyloggingpp)

# 添加crc32c子目录
# add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/crc32c)
