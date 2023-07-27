cmake_minimum_required(VERSION 3.5)
project(sports-tracking VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

set(CMAKE_BUILD_TYPE "Release") 
message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -O3 -march=native")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

message(STATUS "SOURCE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

add_library(file_io SHARED
    ${CMAKE_SOURCE_DIR}/src/file_io.cpp
)

target_include_directories(file_io
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(file_io
    PUBLIC
        ${OpenCV_LIBS}
)

add_library(detector SHARED
    ${CMAKE_SOURCE_DIR}/src/detector.cpp
)

target_include_directories(detector
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(detector
    PUBLIC
        ${OpenCV_LIBS}
)

add_library(timer SHARED
    ${CMAKE_SOURCE_DIR}/src/timer.cpp
)

target_include_directories(timer
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable(unit_test
    ${CMAKE_SOURCE_DIR}/test/unit_test.cpp
)
target_include_directories(unit_test
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(unit_test
    PRIVATE
        file_io 
        detector
        timer
)

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")

add_executable(main
    ${CMAKE_SOURCE_DIR}/main.cpp
)
target_include_directories(main
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(main
    PRIVATE
        file_io 
        detector
        timer
)

endif()