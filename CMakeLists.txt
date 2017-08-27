cmake_minimum_required(VERSION 3.1)

project(mgjson)

include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(msgpack.hpp MSGPACK_HPP_EXISTS)
if (MSGPACK_HPP_EXISTS)
    add_definitions(-DMGJSON_USE_MSGPACK)
endif()

add_subdirectory(tests)
