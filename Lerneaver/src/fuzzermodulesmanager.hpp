#ifndef HEADEAR_FUZZERMODULESMANAGER
#define HEADEAR_FUZZERMODULESMANAGER

#include <map>
#include <memory>
#include <type_traits>

#include "config.hpp"

#include "ModuleManagement/modulesmanager.hpp"
#include "i_fuzzer.hpp"
#include "i_logger.hpp"
#include "i_outputter.hpp"

//====================================================================================================
/**
	\brief Class that manages fuzzers/loggers/outputters modules loading/unloading by their IDs
*/
class CFuzzerModulesManager {
public:
	using TYPE_FUZZERID=config::TYPE_FUZZERID;
	using TYPE_LOGGERID=config::TYPE_LOGGERID;
	using TYPE_OUTPUTTERID=config::TYPE_OUTPUTTERID;
	using TYPE_MODID=config::TYPE_MODID;

private:
	CModulesManager modulesManager;
	std::map<TYPE_FUZZERID, TYPE_MODID> fuzzersModules;
	std::map<TYPE_LOGGERID, TYPE_MODID> loggersModules;
	std::map<TYPE_OUTPUTTERID, TYPE_MODID> outputtersModules;

	template<class TypeEntityID> void unloadEntity(const TypeEntityID &entityID) {
		std::map<TypeEntityID, TYPE_MODID>* entitiesModulesCollection;
		if (std::is_same<TypeEntityID, TYPE_FUZZERID>::value == true) entitiesModulesCollection = &this->fuzzersModules;
		else if (std::is_same<TypeEntityID, TYPE_LOGGERID>::value == true) entitiesModulesCollection = &this->loggersModules;
		else if (std::is_same<TypeEntityID, TYPE_OUTPUTTERID>::value == true) entitiesModulesCollection = &this->outputtersModules;
		try {
			this->modulesManager.unloadModule((*entitiesModulesCollection)[entityID]);
		}
		catch (CModulesManager::ExModulesManager &e) {
			throw ExFuzzerModulesManager(e.getInfo());
		}
		(*entitiesModulesCollection).erase(entityID);
	}

public:
	std::shared_ptr<IFuzzer> loadFuzzerFromModule(const TYPE_FUZZERID &, const TYPE_MODID &);
	void unloadFuzzer(const TYPE_FUZZERID &);
	std::shared_ptr<ILogger> loadLoggerFromModule(const TYPE_LOGGERID &, const TYPE_MODID &);
	void unloadLogger(const TYPE_LOGGERID &);
	std::shared_ptr<IOutputter> loadOutputterFromModule(const TYPE_OUTPUTTERID &, const TYPE_MODID &);
	void unloadOutputter(const TYPE_OUTPUTTERID &);

//Exceptions
	class ExFuzzerModulesManager : public ExEx {
		std::string exInfo;
	public:
		ExFuzzerModulesManager(const std::string &par_exInfo);
		std::string getInfo() const override;
	};
};

#endif
