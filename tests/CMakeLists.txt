cmake_minimum_required(VERSION 3.1)

project(mgjson_test)

enable_testing()

option(MGJSON_USE_QTEST "Using QTest instead of Google Tetst" ON)
if(MGJSON_USE_QTEST)
    find_package(Qt5Core)
    find_package(Qt5Test)
    if(NOT Qt5Core_FOUND OR NOT Qt5Test_FOUND)
        message(WARNING "QT not found, using Google Tests.")
        set(MGJSON_USE_QTEST OFF)
    endif()
endif()

if(MGJSON_USE_QTEST)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    add_executable(${PROJECT_NAME} mgjson_qtest.cpp)
    target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Test)
else()
    ## Loading Google Test in-flight
    include(ExternalProject)

    ExternalProject_Add(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        CMAKE_ARGS
        -DBUILD_GTEST=ON
        PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest"
        INSTALL_COMMAND ""
        )

    # Specify include dir
    ExternalProject_Get_Property(googletest SOURCE_DIR)
    set(GTEST_INCLUDE_DIRS ${SOURCE_DIR}/googletest/include)
    include_directories(${GTEST_INCLUDE_DIRS})

    # Specify MainTest's link libraries
    ExternalProject_Get_Property(googletest BINARY_DIR)
    set(GTEST_LIBS_DIR ${BINARY_DIR}/googlemock/gtest)

    add_executable(${PROJECT_NAME} mgjson_gtest.cpp)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
    add_dependencies(${PROJECT_NAME} googletest)
    add_definitions(-DGTEST_INVOKED)

    if(NOT WIN32 OR MINGW)
        target_link_libraries(${PROJECT_NAME}
            ${GTEST_LIBS_DIR}/libgtest.a
            ${GTEST_LIBS_DIR}/libgtest_main.a
        )
    else()
        target_link_libraries(${PROJECT_TEST_NAME}
            debug ${GTEST_LIBS_DIR}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
            optimized ${GTEST_LIBS_DIR}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
        )
        target_link_libraries(${PROJECT_TEST_NAME}
            debug ${GTEST_LIBS_DIR}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
            optimized ${GTEST_LIBS_DIR}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
        )
    endif()
    if(UNIX)
        target_link_libraries(${PROJECT_NAME} pthread)
    endif()
endif()

add_test(${PROJECT_NAME} ${PROJECT_NAME})
