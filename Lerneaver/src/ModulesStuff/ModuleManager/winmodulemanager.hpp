#ifndef HEADER_WINMODULEMANAGER
#define HEADER_WINMODULEMANAGER

#if defined PLATFORM_WINDOWS

#include "i_modulemanager.hpp"

class CWinModuleManager : public IModuleManager {
	HMODULE hModule;

public:
	CWinModuleManager();
	CWinModuleManager(const TYPE_FILESYSTEMPATH& modulePath);
	void loadModule(const TYPE_FILESYSTEMPATH& modulePath) override;
	void unloadModule();
	TYPE_SIZE getFunctionAddress(const std::string& functionExportName) override;
	// Get function addr by ordinal
	TYPE_SIZE getFunctionAddress(WORD functionOrdinal);

	~CWinModuleManager();
};

#endif

#endif