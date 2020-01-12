#include "winmodulecontainer.hpp"

//WinModuleContainer
//====================================================================================================
CWinModuleContainer::CWinModuleContainer(): hModule(NULL) {}

//====================================================================================================
CWinModuleContainer::CWinModuleContainer(const LPCSTR &modulePath): hModule(NULL) {
	this->loadModule(modulePath);
}

//====================================================================================================
void CWinModuleContainer::loadModule(const LPCSTR &modulePath) {
	this->unloadModule();
	auto hModule = LoadLibrary(reinterpret_cast<LPCTSTR>(modulePath));
	if (NULL == hModule) throw ExWinModuleContainer("Failed to load library");
	this->hModule = hModule;
}

//====================================================================================================
void CWinModuleContainer::unloadModule() {
	if (NULL == this->hModule) return;
	FreeLibrary(this->hModule);
	this->hModule = NULL;
}

//====================================================================================================
TYPE_SIZE CWinModuleContainer::getFunctionAddress(const LPCSTR &functionExportName) {
	auto functionAddr = reinterpret_cast<TYPE_SIZE>(GetProcAddress(this->hModule, functionExportName));
	if (NULL == functionAddr) throw ExWinModuleContainer("Failed to get module function address");
	return functionAddr;
}

//====================================================================================================
TYPE_SIZE CWinModuleContainer::getFunctionAddress(WORD functionOrdinal) {
	auto functionAddr = reinterpret_cast<TYPE_SIZE>(GetProcAddress(this->hModule, reinterpret_cast<LPCSTR>(functionOrdinal)));
	if (NULL == functionAddr) throw ExWinModuleContainer("Failed to get module function address (by ordinal)");
	return functionAddr;
}

//====================================================================================================
CWinModuleContainer::~CWinModuleContainer() {
	this->unloadModule();
}

//CWinModuleContainerRAII
//====================================================================================================
CWinModuleContainerRAII::CWinModuleContainerRAII(const LPCSTR &modulePath) {
	auto hModule = LoadLibrary(reinterpret_cast<LPCTSTR>(modulePath));
	if (NULL == hModule) throw ExWinModuleContainer("Failed to load library");
	this->hModule = hModule;
}

//====================================================================================================
TYPE_SIZE CWinModuleContainerRAII::getFunctionAddress(const LPCSTR &functionExportName) {
	auto functionAddr = reinterpret_cast<TYPE_SIZE>(GetProcAddress(this->hModule, functionExportName));
	if (NULL == functionAddr) throw ExWinModuleContainer("Failed to get module function address");
	return functionAddr;
}

//====================================================================================================
TYPE_SIZE CWinModuleContainerRAII::getFunctionAddress(WORD functionOrdinal) {
	auto functionAddr = reinterpret_cast<TYPE_SIZE>(GetProcAddress(this->hModule, reinterpret_cast<LPCSTR>(functionOrdinal)));
	if (NULL == functionAddr) throw ExWinModuleContainer("Failed to get module function address (by ordinal)");
	return functionAddr;
}

CWinModuleContainerRAII::~CWinModuleContainerRAII() {
	FreeLibrary(this->hModule);
}

//Exceptions
//====================================================================================================
ExWinModuleContainer::ExWinModuleContainer(const std::string &par_msg): _msg(par_msg), _errorCode(GetLastError()) {}

std::string ExWinModuleContainer::getInfo() const {
	return std::string("[CWinModuleContainer EXCEPTION] ") + _msg + " err_code:" + std::to_string(_errorCode); //TODO is std::to_string necessary?
}
