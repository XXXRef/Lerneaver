#ifndef HEADER_I_MODULEMANAGER
#define HEADER_I_MODULEMANAGER

#include <string>
#include <exception>

#include "types.hpp"
#include "utils.hpp"

/**
	\brief Module owner interface
*/
class IModuleManager {
public:
	virtual void loadModule(const TYPE_FILESYSTEMPATH& modulePath) = 0;
	virtual void unloadModule() = 0;
	virtual TYPE_SIZE getFunctionAddress(const std::string& functionName) = 0;
	virtual ~IModuleManager() {}

	// Exceptions
	class ExModuleManager : public ExException {};
};

#endif