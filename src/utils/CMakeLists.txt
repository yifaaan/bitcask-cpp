add_library(utils)

target_sources(utils 
PRIVATE common.cpp
PUBLIC common.h
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})