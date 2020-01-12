#ifndef HEADER_WINMODULECONTAINER
#define HEADER_WINMODULECONTAINER

#include "i_modulecontainer.hpp"

#include <exception>

#include <windows.h>

//=====================================================================================================================
class CWinModuleContainer: public IModuleContainer {
	HMODULE hModule;

public:
	CWinModuleContainer();
	CWinModuleContainer(const LPCSTR &modulePath);
	void loadModule(const LPCSTR &modulePath) override;
	void unloadModule() override;
	TYPE_SIZE getFunctionAddress(const LPCSTR &functionExportName) override;
	TYPE_SIZE getFunctionAddress(WORD functionOrdinal);// Get function address by ordinal
	~CWinModuleContainer() override;
};

//=====================================================================================================================
class CWinModuleContainerRAII : public IModuleContainerRAII {
	HMODULE hModule;

public:
	CWinModuleContainerRAII(const LPCSTR &modulePath);
	TYPE_SIZE getFunctionAddress(const LPCSTR &) override;
	TYPE_SIZE getFunctionAddress(WORD functionOrdinal);// Get function address by ordinal
	~CWinModuleContainerRAII() override;
};

//Exceptions
//=====================================================================================================================
class ExWinModuleContainer : public ExModuleContainer {
public:
	DWORD _errorCode;
	std::string _msg;

	ExWinModuleContainer(const std::string &par_msg = "");
	std::string getInfo() const override;
};

#endif
