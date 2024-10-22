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
if(NOT DEFINED FLAG_BUILD_LERNEAVER) #TODO KOSTIL'
	add_subdirectory("../Lerneaver" "${CMAKE_CURRENT_BINARY_DIR}/Lerneaver") #TIP generated project files for Lerneaver lib placed in its own "Lerneaver" subdir
endif()

#Compile
add_executable(${PROJECT_NAME} ${SRC_FILES})

#Link with Lerneaver lib
target_link_libraries(${PROJECT_NAME} "Lerneaver")
