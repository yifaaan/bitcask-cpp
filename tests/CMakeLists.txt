add_executable(test_db test_db.cpp)

target_include_directories(test_db PRIVATE ${CMAKE_SOURCE_DIR}/third_party/catch)