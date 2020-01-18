#include "logger.hpp"

#if defined PLATFORM_WINDOWS
extern "C" {
	__declspec(dllexport) ILogger* getLogger();
}

ILogger* getLogger() {
	return new CLogger();
}
#elif defined PLATFORM_UNIX
extern "C" ILogger* getLogger() {
	return new CLogger();
}
#endif

#if defined PLATFORM_WINDOWS
#ifdef _DEBUG
BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

#endif
#endif