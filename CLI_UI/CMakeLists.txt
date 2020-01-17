cmake_minimum_required(VERSION 2.8)

#Suppress ZERO_CHECK prject generation
set(CMAKE_SUPPRESS_REGENERATION true)

#There is no way to suppress ALL_BUILD project generation (for "make all")
set(PROJECT_NAME "CLI_UI")

if(WIN32)
    set(PLATFORM_ID PLATFORM_WINDOWS)
elseif(UNIX)
    set(PLATFORM_ID PLATFORM_NIX)
elseif(APPLE)
    set(PLATFORM_ID PLATFORM_MACOS)
endif()
add_compile_definitions(${PLATFORM_ID})

include_directories("./src" "../Lerneaver/src/inc" "../Lerneaver/src/inc/EXT")

file(GLOB SRC_FILES "./src/*.cpp")

#Build Lerneaver library
add_subdirectory("../Lerneaver" "./BUILD") #WARNING Is it permitted? Subproject directory on the same lvl as root project

#Compile
add_executable(${PROJECT_NAME} ${SRC_FILES})

#Link with Lerneaver lib
target_link_libraries(${PROJECT_NAME} "./BUILD/Lerneaver")