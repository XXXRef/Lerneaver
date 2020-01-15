cmake_minimum_required(VERSION 2.8)

#Suppress ZERO_CHECK project generation
set(CMAKE_SUPPRESS_REGENERATION true)

set(PROJECT_NAME Lerneaver)

project(${PROJECT_NAME})

if(WIN32)
    set(PLATFORM_ID PLATFORM_WINDOWS)
elseif(UNIX)
    set(PLATFORM_ID PLATFORM_NIX)
elseif(APPLE)
    set(PLATFORM_ID PLATFORM_MACOS)
endif()
add_compile_definitions(${PLATFORM_ID})

include_directories(./src/inc ./src/inc/EXT)

#Add all src files in src folder recursively
file(GLOB_RECURSE SRC_FILES ./src *.cpp)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})