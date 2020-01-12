#ifndef HEADER_I_MODULECONTAINER
#define HEADER_I_MODULECONTAINER

#include <string>

#include "utils.hpp"
#include "types.hpp"
#include "config.hpp"

//====================================================================================================
/**
	Particular module container class
*/

class IModuleContainer {
public:
	using TYPE_FUNCNAME = config::platform::TYPE_FUNCNAME;
	using TYPE_MODID = config::TYPE_MODID;

	virtual void loadModule(const TYPE_MODID &) = 0;
	virtual void unloadModule() = 0;
	virtual TYPE_SIZE getFunctionAddress(const TYPE_FUNCNAME &) = 0;
	virtual ~IModuleContainer() {}
};

//====================================================================================================
/**
	Particular module container class using RAII-concept
*/
class IModuleContainerRAII {
public:
	using TYPE_FUNCNAME = config::platform::TYPE_FUNCNAME;

	virtual TYPE_SIZE getFunctionAddress(const TYPE_FUNCNAME &) = 0;
	virtual ~IModuleContainerRAII() {}
};

// Module container exception class
//====================================================================================================
class ExModuleContainer : public ExEx {};
#endif
