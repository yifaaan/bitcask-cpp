add_executable(main main.cpp)

add_subdirectory(storage)
add_subdirectory(utils)

target_link_libraries(main
PUBLIC
    storage
PUBLIC
    utils
)