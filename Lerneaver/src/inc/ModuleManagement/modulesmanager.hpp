#ifndef HEADER_MODULESMANAGER
#define HEADER_MODULESMANAGER

#include <map>
#include <utility>
#include <string>
#include <memory>
#include <exception>

#include "types.hpp"
#include "utils.hpp"
#include "ModuleContainer/i_modulecontainer.hpp"

#include "config.hpp"

//TODO Cos of include loop this config part must be here

#if defined PLATFORM_WINDOWS
#include "ModuleContainer/winmodulecontainer.hpp"
namespace config {
	using TYPE_MODULECONTAINER = CWinModuleContainer;
#elif defined PLATFORM_NIX
namespace config {
	//TODO
#endif
}

//====================================================================================================
/**
	\brief Handles job modules

	ModulesContainer class is intentionally platform-independent. All platform-specific business is done in appropriate ModuleContainer classes
*/
class CModulesManager {
public: // public types
	using TYPE_MODID = config::TYPE_MODID; //just not to write "config::"; user code has access to this interface functions param type through config

protected:
	/**
		Map storing module-{moduleHandle:module_refs_amount} association
	*/
	std::map<TYPE_MODID, std::pair<std::shared_ptr<IModuleContainer>, TYPE_SIZE>> modules;

public:
	virtual ~CModulesManager();
	/**
		Loads module if its not done yet; if module already loaded, increments its refs amount
	*/
	void loadModule(const TYPE_MODID &);
	/**
		Decrements module refs amount and unloads it if amount becomes 0
	*/
	void unloadModule(const TYPE_MODID &);

	std::shared_ptr<IModuleContainer> operator[](const TYPE_MODID &) const;

	//Exceptions
	class ExModulesManager : public ExEx { //TODO Rename of ExModulesContainer
		std::string exInfo;
	public:
		ExModulesManager(const std::string & par_exInfo);

		std::string getInfo() const override;
	};
};

#endif
