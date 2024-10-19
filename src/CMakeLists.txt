add_executable(dbserver 
main.cpp file.cpp
record.cpp
active_file.cpp
active_map.cpp
record.cpp
record_map.cpp
stable_file.cpp
)

add_subdirectory(storage)
add_subdirectory(utils)

# header only
# add_library(cache INTERFACE)
# target_include_directories(cache INTERFACE .)

# 添加 third_party 目录中包含的头文件路径
target_include_directories(dbserver 
PUBLIC
    ${PROJECT_SOURCE_DIR}/third_party/include
PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)




find_library(crc crc32c ${PROJECT_SOURCE_DIR}/third_party/lib)
find_library(el easyloggingpp ${PROJECT_SOURCE_DIR}/third_party/lib)
# 链接第三方库（静态库或共享库）
target_link_libraries(dbserver
PRIVATE
    storage
PRIVATE
    gtest_main
    PRIVATE
    ${crc}
    PRIVATE
    ${el}
    # PRIVATE
    # cache
)



# enable_testing()

# include(GoogleTest)
# gtest_discover_tests(dbserver)
