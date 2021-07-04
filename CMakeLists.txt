cmake_minimum_required(VERSION 3.10) 

# SET(CMAKE_BUILD_TYPE "Debug")

project(rawjson) 

add_library(rawjson STATIC
    serialize/branchlut2.c
    serialize/rawjson_serialize.c
)

target_include_directories(rawjson
    PUBLIC include
    PRIVATE serialize
)

add_executable(test_rawjson
    test/test.c
)

target_include_directories(test_rawjson
    PUBLIC include
    PRIVATE serialize
)

target_link_libraries(test_rawjson PRIVATE
    rawjson
    m
)
