cmake_minimum_required(VERSION 3.24)

project(bitcask)

set(CMAKE_CXX_STANDARD 20)
# 可执行文件
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
# 静态库
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
# 动态库
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)



#添加一个包含 CMakeLists.txt 的子目录
add_subdirectory(src)
#添加一个包含 CMakeLists.txt 的子目录
add_subdirectory(tests)



# add_executable(catch_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_db.cpp ${CMAKE_CURRENT_SOURCE_DIR}/third_party/catch.hpp)