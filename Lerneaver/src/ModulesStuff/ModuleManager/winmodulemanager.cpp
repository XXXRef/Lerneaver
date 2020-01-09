#if defined PLATFORM_WINDOWS

#include "winmodulemanager.hpp"

//====================================================================================================
CWinModuleManager::CWinModuleManager() : hModule(NULL){}

CWinModuleManager::CWinModuleManager(const TYPE_FILESYSTEMPATH& modulePath): hModule(NULL) {
	this->loadModule(modulePath);
}

//====================================================================================================
void CWinModuleManager::loadModule(const TYPE_FILESYSTEMPATH& modulePath) {
	this->unloadModule();
	HMODULE hModule = LoadLibrary((LPCTSTR)modulePath.c_str());
	if (hModule==NULL) {
		throw ExWindowsError("Failed to load library");
	}
	this->hModule = hModule;
}

//====================================================================================================
void CWinModuleManager::unloadModule() {
	if (this->hModule==NULL) {
		return;
	}
	FreeLibrary(this->hModule);
	this->hModule = NULL;
}

//====================================================================================================
TYPE_SIZE CWinModuleManager::getFunctionAddress(const std::string& functionName) {
	TYPE_SIZE functionAddr = (TYPE_SIZE)GetProcAddress(this->hModule, functionName.c_str());
	if (functionAddr==NULL) {
		throw ExWindowsError("GetProcAddress failed");
	}
	return functionAddr;
}

//====================================================================================================
// Get function addr by ordinal
TYPE_SIZE CWinModuleManager::getFunctionAddress(WORD functionOrdinal) {
	TYPE_SIZE functionAddr = (TYPE_SIZE)GetProcAddress(this->hModule,(LPCSTR)functionOrdinal);
	if (functionAddr==NULL) {
		throw ExWindowsError("GetProcAddress (by ordinal) failed");
	}
	return functionAddr;
}

//====================================================================================================
CWinModuleManager::~CWinModuleManager() {
	this->unloadModule();
}

#endif