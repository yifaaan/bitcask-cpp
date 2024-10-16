add_executable(dbserver main.cpp)

add_subdirectory(storage)
add_subdirectory(utils)

target_link_libraries(dbserver
PUBLIC
    storage
    GTest::gtest_main
)



enable_testing()

# add_executable(
#   hello_test
#   hello_test.cc
# )
# target_link_libraries(
#     dbserver
  
# )

include(GoogleTest)
gtest_discover_tests(dbserver)
