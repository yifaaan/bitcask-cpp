add_library(storage)

target_sources(storage 
PRIVATE server.cpp
PUBLIC server.h
)
target_include_directories(storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_link_libraries(storage PUBLIC utils)
