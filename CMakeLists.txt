cmake_minimum_required(VERSION 3.10) 

SET(CMAKE_BUILD_TYPE "Debug")
add_subdirectory(yyjson/)

project(rawjson_ser) 

add_library(rawjson_ser STATIC
    # serialize/branchlut2.c
    serialize/interger.c
    serialize/rawjson_serialize.c
)

target_include_directories(rawjson_ser
    PUBLIC include
    PRIVATE serialize
)

project(rawjson_der)

add_library(rawjson_der STATIC
    deserialize/rawjson_deserialize.c
)

target_include_directories(rawjson_der
    PUBLIC include
    PRIVATE deserialize
)

enable_testing()

add_executable(test_deserialize
    test/test_derserialize.c
)

target_link_libraries(test_deserialize
    rawjson_der
    yyjson
)

add_test(NAME deserialize COMMAND test_deserialize rfc3399)
