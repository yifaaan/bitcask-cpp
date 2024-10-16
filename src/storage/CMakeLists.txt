add_library(server)

target_sources(server 
PRIVATE server.cpp
PUBLIC server.h
)