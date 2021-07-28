cmake_minimum_required(VERSION 3.10) 

# SET(CMAKE_BUILD_TYPE "Debug")

project(rawjson) 

add_library(rawjson_ser STATIC
    # serialize/branchlut2.c
    serialize/interger.c
    serialize/rawjson_serialize.c
)

target_include_directories(rawjson_ser
    PUBLIC include
    PRIVATE serialize
)
