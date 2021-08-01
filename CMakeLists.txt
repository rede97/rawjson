cmake_minimum_required(VERSION 3.10) 

# SET(CMAKE_BUILD_TYPE "Debug")
add_subdirectory(yyjson/)

project(rawjson_ser) 

add_library(rawjson_ser SHARED
    # serialize/branchlut2.c
    serialize/interger.c
    serialize/rawjson_serialize.c
)

target_include_directories(rawjson_ser
    PUBLIC include
    PRIVATE serialize
)

project(rawjson_der)

add_library(rawjson_der SHARED
    deserialize/rawjson_deserialize.c
)

target_include_directories(rawjson_der
    PUBLIC include
    PRIVATE deserialize
)

# target_link_libraries(rawjson_der
#     yyjson
# )
