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

	enum ENTITIES_IDS {FUZZER,LOGGER,OUTPUTTER};

private:
	CModulesManager modulesManager;
	std::map<TYPE_FUZZERID, TYPE_MODID> fuzzersModules;
	std::map<TYPE_LOGGERID, TYPE_MODID> loggersModules;
	std::map<TYPE_OUTPUTTERID, TYPE_MODID> outputtersModules;

	template<ENTITIES_IDS entityID, class TypeEntity, class TypeEntityID> std::shared_ptr<TypeEntity> loadEntityFromModule(const TypeEntityID &entityID, const TYPE_MODID &entityModID) {
		std::map<TypeEntityID, TYPE_MODID> *entitiesModulesCollection;
		const config::platform::TYPE_FUNCNAME *entityFactoryFuncName;
		std::string entityName;
		//TIP logic based on particular entity type
		if (ENTITIES_IDS::FUZZER == entityID) {
			entitiesModulesCollection = &this->fuzzersModules; 
			entityFactoryFuncName = &config::fuzzerFactoryFunctionName;
			entityName = "fuzzer";
		}
		else if (ENTITIES_IDS::LOGGER == entityID) {
			entitiesModulesCollection = &this->loggersModules;
			entityFactoryFuncName = &config::loggerFactoryFunctionName;
			entityName = "logger";
		}
		else if (ENTITIES_IDS::OUTPUTTER == entityID) {
			entitiesModulesCollection = &this->outputtersModules;
			entityFactoryFuncName = &config::outputterFactoryFunctionName;
			entityName = "outputter";
		}
		//Load entity module
		try {
			this->modulesManager.loadModule(entityModID);
		}
		catch (CModulesManager::ExModulesManager &e) {
			throw ExFuzzerModulesManager(e.getInfo());
		}
		entitiesModulesCollection->insert({ entityID,entityModID });
		//Get entity obj factory function
		TypeEntity*(*pfnEntityFactoryFunction)();
		try {
			pfnEntityFactoryFunction = reinterpret_cast<TypeEntity*(*)()>(this->modulesManager[entityModID]->getFunctionAddress(*entityFactoryFuncName));
		}
		catch (CModulesManager::ExModulesManager &e) {
			throw ExFuzzerModulesManager(e.getInfo());
		}
		//Acquire outputter obj from factory
		auto pEntityObj = std::shared_ptr<TypeEntity>(pfnEntityFactoryFunction());
		if (nullptr == pEntityObj) throw ExFuzzerModulesManager(std::string("Failed to acquire ")+ entityName + " object from module");
		return pEntityObj;
	}

	template<ENTITIES_IDS entityID, class TypeEntityID> void unloadEntity(const TypeEntityID &entityID) {
		std::map<TypeEntityID, TYPE_MODID>* entitiesModulesCollection;
		//TIP logic based on particular entity type
		if (ENTITIES_IDS::FUZZER == entityID) entitiesModulesCollection = &this->fuzzersModules;
		else if (ENTITIES_IDS::LOGGER == entityID) entitiesModulesCollection = &this->loggersModules;
		else if (ENTITIES_IDS::OUTPUTTER == entityID) entitiesModulesCollection = &this->outputtersModules;
		try {
			this->modulesManager.unloadModule((*entitiesModulesCollection)[entityID]);
		}
		catch (CModulesManager::ExModulesManager &e) {
			throw ExFuzzerModulesManager(e.getInfo());
		}
		(*entitiesModulesCollection).erase(entityID);
	}

public:
	std::shared_ptr<IFuzzer> loadFuzzerFromModule(const TYPE_FUZZERID &fuzzerID, const TYPE_MODID &fuzzerModID) {
		return this->loadEntityFromModule<ENTITIES_IDS::FUZZER, IFuzzer>(fuzzerID, fuzzerModID);
	}
	void unloadFuzzer(const TYPE_FUZZERID &fuzzerID) {
		return this->unloadEntity<ENTITIES_IDS::FUZZER>(fuzzerID);
	}
	std::shared_ptr<ILogger> loadLoggerFromModule(const TYPE_LOGGERID &loggerID, const TYPE_MODID &loggerModID) {
		return this->loadEntityFromModule<ENTITIES_IDS::LOGGER,ILogger>(loggerID, loggerModID);
	}
	void unloadLogger(const TYPE_LOGGERID &loggerID) {
		this->unloadEntity<ENTITIES_IDS::LOGGER>(loggerID);
	}
	std::shared_ptr<IOutputter> loadOutputterFromModule(const TYPE_OUTPUTTERID &outputterID, const TYPE_MODID &outputterModID) {
		return this->loadEntityFromModule<ENTITIES_IDS::OUTPUTTER,IOutputter>(outputterID, outputterModID);
	}
	void unloadOutputter(const TYPE_OUTPUTTERID &outputterID) {
		this->unloadEntity<ENTITIES_IDS::LOGGER>(outputterID);
	}

//Exceptions
	class ExFuzzerModulesManager : public ExEx {
		std::string exInfo;
	public:
		ExFuzzerModulesManager(const std::string &par_exInfo) : exInfo(par_exInfo) {}
		std::string getInfo() const override {
			return std::string("[CFuzzerModulesManager EXCEPTION] ") + exInfo;
		}
	};
};

#endif
