cmake_minimum_required(VERSION 2.8)

#Suppress ZERO_CHECK prject generation
set(CMAKE_SUPPRESS_REGENERATION true)

set(PROJECT_NAME "TestOutputter")

if(WIN32)
    set(PLATFORM_ID PLATFORM_WINDOWS)
elseif(UNIX)
    set(PLATFORM_ID PLATFORM_NIX)
elseif(APPLE)
    set(PLATFORM_ID PLATFORM_MACOS)
endif()
add_compile_definitions(${PLATFORM_ID})

include_directories("./src" "./src/inc")

file(GLOB SRC_FILES "./src/*.cpp")

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
