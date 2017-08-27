cmake_minimum_required(VERSION 3.1)

project(mgjson)

find_package(mgjson 0.0.1 REQUIRED HINTS ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(tests)
