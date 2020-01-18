cmake_minimum_required(VERSION 2.8)

#Suppress ZERO_CHECK prject generation
set(CMAKE_SUPPRESS_REGENERATION true)

set(PROJECT_NAME "Learneaver-all")

project(${PROJECT_NAME})

option (FLAG_ADD_LERNEAVER "ADD_LERNEAVER" ON)
option (FLAG_ADD_CLI_UI "ADD_CLI_UI" ON)
option (FLAG_ADD_TESTFUZZER "ADD_TESTFUZZER" ON)
option (FLAG_ADD_TESTLOGGER "ADD_TESTLOGGER" ON)
option (FLAG_ADD_TESTOUTPUTTER "ADD_TESTOUTPUTTER" ON)

message("OiledOwl production..")

#TIP how to deal with string cmd args
#set(FLAG_P1 "ADD_P1" CACHE STRING "TRUE")
#if(${FLAG_P1} STREQUAL "TRUE")
#	 add_subdirectory("./P1")
#endif()
#
#cmake -DFLAG_P1:STRING=TRUE project_dir

#Build Lerneaver
if(${FLAG_ADD_LERNEAVER}) 
	set(FLAG_BUILD_LERNEAVER ON)#WARNING KOSTIL'
	add_subdirectory("./Lerneaver")
endif()

#Build CLI_UI
if(${FLAG_ADD_CLI_UI})
	add_subdirectory("./CLI_UI") #TIP CLI_UI links (have to) with Lerneaver itself
endif()

#Build TestFuzzer/TestLogger/TestOutputter
if(${FLAG_ADD_TESTFUZZER})
	add_subdirectory("./TestFuzzer")
endif()
if(${FLAG_ADD_TESTLOGGER})
	add_subdirectory("./TestLogger")
endif()
if(${FLAG_ADD_TESTOUTPUTTER})
	add_subdirectory("./TestOutputter")
endif()
