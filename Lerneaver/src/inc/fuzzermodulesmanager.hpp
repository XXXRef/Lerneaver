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

	template<class TypeEntity, class TypeEntityID> std::shared_ptr<TypeEntity> loadEntityFromModule(const TypeEntityID &entityID, const TYPE_MODID &entityModID) {
		std::map<TypeEntityID, TYPE_MODID> *entitiesModulesCollection;
		const config::platform::TYPE_FUNCNAME *entityFactoryFuncName;
		std::string entityName;
		//TIP logic based on particular entity type
		if (std::is_same<TypeEntityID, TYPE_FUZZERID>::value == true) { 
			entitiesModulesCollection = &this->fuzzersModules; 
			entityFactoryFuncName = &config::fuzzerFactoryFunctionName;
			entityName = "fuzzer";
		}
		else if (std::is_same<TypeEntityID, TYPE_LOGGERID>::value == true) {
			entitiesModulesCollection = &this->loggersModules;
			entityFactoryFuncName = &config::loggerFactoryFunctionName;
			entityName = "logger";
		}
		else if (std::is_same<TypeEntityID, TYPE_OUTPUTTERID>::value == true) {
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

	template<class TypeEntityID> void unloadEntity(const TypeEntityID &entityID) {
		std::map<TypeEntityID, TYPE_MODID>* entitiesModulesCollection;
		//TIP logic based on particular entity type
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
	std::shared_ptr<IFuzzer> loadFuzzerFromModule(const TYPE_FUZZERID &fuzzerID, const TYPE_MODID &fuzzerModID) {
		return this->loadEntityFromModule<IFuzzer>(fuzzerID, fuzzerModID);
	}
	void unloadFuzzer(const TYPE_FUZZERID &fuzzerID) {
		return this->unloadEntity(fuzzerID);
	}
	std::shared_ptr<ILogger> loadLoggerFromModule(const TYPE_LOGGERID &loggerID, const TYPE_MODID &loggerModID) {
		return this->loadEntityFromModule<ILogger>(loggerID, loggerModID);
	}
	void unloadLogger(const TYPE_LOGGERID &loggerID) {
		this->unloadEntity(loggerID);
	}
	std::shared_ptr<IOutputter> loadOutputterFromModule(const TYPE_OUTPUTTERID &outputterID, const TYPE_MODID &outputterModID) {
		return this->loadEntityFromModule<IOutputter>(outputterID, outputterModID);
	}
	void unloadOutputter(const TYPE_OUTPUTTERID &outputterID) {
		this->unloadEntity(outputterID);
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
