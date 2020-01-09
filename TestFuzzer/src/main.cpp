#include "fuzzer/fuzzer.hpp"

extern "C" {
	__declspec(dllexport) IFuzzer* getFuzzer();
}

IFuzzer* getFuzzer() {
	return new CFuzzer();
}

#ifdef _DEBUG
BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
	switch (fdwReason){
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