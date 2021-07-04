cmake_minimum_required(VERSION 3.10) 

project(rawjson) 

SET(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_C_FLAGS "-O3")

add_executable(rawjson 
    serialize/branchlut2.c
    serialize/rawjson_serialize.c
    serialize/test.c
)

target_include_directories(rawjson
    PUBLIC include
    PRIVATE serialize
)

target_link_libraries(rawjson PRIVATE
    m
)
