#ifndef HEADER_CONFIG
#define HEADER_CONFIG

#include <string>

//include common platform-specific headers
#if defined PLATFORM_WINDOWS
	#include <windows.h>
#elif defined PLATFORM_NIX	
	//TODO
#endif

namespace config {
	namespace platform {
#if defined PLATFORM_WINDOWS
		using TYPE_FILESYSTEMPATH = LPCSTR;
		using TYPE_FUNCNAME = LPCSTR;
#elif defined PLATFORM_NIX
		//TODO
#endif
	}

	using TYPE_FUZZERID = std::string;
	using TYPE_LOGGERID = std::string;
	using TYPE_OUTPUTTERID = std::string;

	using TYPE_MODID = platform::TYPE_FILESYSTEMPATH;//TIP actually here should be 3 types: TYPE_FUZZERMODID, TYPE_LOGGERMODID, TYPE_OUTPUTTERMODID
	using TYPE_CONFIGID = platform::TYPE_FILESYSTEMPATH;//TIP same here; 3 types: TYPE_FUZZERCONFIGID,TYPE_LOGGERCONFIGID, TYPE_OUTPUTTERCONFIGID

//Factory function names has type platform::TYPE_FUNCNAME => they are implicitly platform-dependent but they shouldnt belong to "platform" namespace
#if defined PLATFORM_WINDOWS 
	const platform::TYPE_FUNCNAME fuzzerFactoryFunctionName = "getFuzzer"; // IFuzzer* getFuzzer()
	const platform::TYPE_FUNCNAME loggerFactoryFunctionName = "getLogger"; //ILogger* getLogger()
	const platform::TYPE_FUNCNAME outputterFactoryFunctionName = "getOutputter"; //IOutputter* getOutputter()
#elif defined PLATFORM_NIX

#endif

}

#endif
