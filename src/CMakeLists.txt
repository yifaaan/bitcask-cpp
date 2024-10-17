add_executable(dbserver main.cpp)

add_subdirectory(storage)
add_subdirectory(utils)


# 添加 third_party 目录中包含的头文件路径
target_include_directories(dbserver PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/include
)

# 链接第三方库（静态库或共享库）
target_link_libraries(dbserver
PRIVATE
    storage
PRIVATE
    # ${CMAKE_SOURCE_DIR}/third_party/lib/gtest_main
    gtest_main
)



enable_testing()



include(GoogleTest)
gtest_discover_tests(dbserver)
