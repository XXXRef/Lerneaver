#ifndef HEADER_MODULESCONTAINER
#define HEADER_MODULESCONTAINER

#include <map>
#include <utility>
#include <string>
#include <exception>
#include <memory>

#include "types.hpp"
#include "utils.hpp"
#include "ModuleManager/i_modulemanager.hpp"

#if defined PLATFORM_WINDOWS
#include "ModuleManager/winmodulemanager.hpp"
using CLASS_MODULEMANAGER = CWinModuleManager;
#elif defined PLATFORM_UNIX

#elif defined PLATFORM_MAC

#endif

/**
	Class for managing dynamic modules loading/unloading
*/
class CModulesContainer {
protected:
	std::map<TYPE_FILESYSTEMPATH, std::pair<std::shared_ptr<IModuleManager>, TYPE_SIZE>> modules; // {<lib_path>: {IModuleManager, <lib_use_count>}}

public:
	virtual ~CModulesContainer() {}
	void loadModule(const TYPE_FILESYSTEMPATH& libPath);
	void unloadModule(const TYPE_FILESYSTEMPATH& libPath);

	std::shared_ptr<IModuleManager> operator[](const TYPE_FILESYSTEMPATH& libPath);

	//Types
	class ExNoModulesFound : public std::exception {};
};

#endif