#include "fuzzermodulesmanager.hpp"

//CFuzzerModulesContainer
//====================================================================================================
std::shared_ptr<IFuzzer> CFuzzerModulesManager::loadFuzzerFromModule(const TYPE_FUZZERID &fuzzerID, const TYPE_MODID &fuzzerModID) {
	//Load fuzzer module
	try{
		this->modulesManager.loadModule(fuzzerModID);
	}
	catch (CModulesManager::ExModulesManager & e) {
		throw ExFuzzerModulesManager(e.getInfo());
	}
	this->fuzzersModules.insert({ fuzzerID,fuzzerModID });
	//Get fuzzer obj factory function
	IFuzzer* (*pfnFuzzerFactoryFunction)();
	try{
		pfnFuzzerFactoryFunction = reinterpret_cast<IFuzzer*(*)()>(this->modulesManager[fuzzerModID]->getFunctionAddress(config::fuzzerFactoryFunctionName));
	}
	catch (CModulesManager::ExModulesManager &e) {
		throw ExFuzzerModulesManager(e.getInfo());
	}
	//Acquire fuzzer obj from factory
	auto pFuzzerObj=std::shared_ptr<IFuzzer>(pfnFuzzerFactoryFunction());
	if (nullptr == pFuzzerObj) throw ExFuzzerModulesManager("Failed to acquire fuzzer object from module");
	return pFuzzerObj;
}

//====================================================================================================
void CFuzzerModulesManager::unloadFuzzer(const TYPE_FUZZERID &fuzzerID) {
	try{
		this->modulesManager.unloadModule(this->fuzzersModules[fuzzerID]);
	}
	catch (CModulesManager::ExModulesManager &e) {
		throw ExFuzzerModulesManager(e.getInfo());
	}
	this->fuzzersModules.erase(fuzzerID);
}

//====================================================================================================
std::shared_ptr<ILogger> CFuzzerModulesManager::loadLoggerFromModule(const TYPE_LOGGERID &loggerID, const TYPE_MODID &loggerModID) {
	//Load logger module
	try{
		this->modulesManager.loadModule(loggerModID);
	}
	catch (CModulesManager::ExModulesManager &e) {
		throw ExFuzzerModulesManager(e.getInfo());
	}
	this->loggersModules.insert({ loggerID, loggerModID });
	//Get logger obj factory function
	ILogger* (*pfnLoggerFactoryFunction)();
	try {
		pfnLoggerFactoryFunction = reinterpret_cast<ILogger*(*)()>(this->modulesManager[loggerModID]->getFunctionAddress(config::loggerFactoryFunctionName));
	}
	catch (CModulesManager::ExModulesManager &e) {
		throw ExFuzzerModulesManager(e.getInfo());
	}
	//Acquire logger obj from factory
	auto pLoggerObj = std::shared_ptr<ILogger>(pfnLoggerFactoryFunction());
	if (nullptr == pLoggerObj) throw ExFuzzerModulesManager("Failed to acquire logger object from module");
	return pLoggerObj;
}

//====================================================================================================
void CFuzzerModulesManager::unloadLogger(const TYPE_LOGGERID &loggerID) {
	try {
		this->modulesManager.unloadModule(this->loggersModules[loggerID]);
	}
	catch (CModulesManager::ExModulesManager &e) {
		throw ExFuzzerModulesManager(e.getInfo());
	}
	this->loggersModules.erase(loggerID);
}

//====================================================================================================
std::shared_ptr<IOutputter> CFuzzerModulesManager::loadOutputterFromModule(const TYPE_OUTPUTTERID &outputterID, const TYPE_MODID &outputterModID) {
	//Load outputter module
	try{
		this->modulesManager.loadModule(outputterModID);
	}
	catch (CModulesManager::ExModulesManager &e) {
		throw ExFuzzerModulesManager(e.getInfo());
	}
	this->outputtersModules.insert({ outputterID,outputterModID });
	//Get outputter obj factory function
	IOutputter*(*pfnOutputterFactoryFunction)();
	try {
		pfnOutputterFactoryFunction = reinterpret_cast<(IOutputter*(*)()>(this->modulesManager[outputterModID]->getFunctionAddress(config::outputterFactoryFunctionName));
	}
	catch (CModulesManager::ExModulesManager &e) {
		throw ExFuzzerModulesManager(e.getInfo());
	}
	//Acquire outputter obj from factory
	auto pOutputterObj = std::shared_ptr<IOutputter>(pfnOutputterFactoryFunction());
	if (nullptr == pOutputterObj) throw ExFuzzerModulesManager("Failed to acquire outputter object from module");
	return pOutputterObj;
}

//====================================================================================================
void CFuzzerModulesManager::unloadOutputter(const TYPE_OUTPUTTERID &outputterID) {
	try{
		this->modulesManager.unloadModule(this->outputtersModules[outputterID]);
	}
	catch (CModulesManager::ExModulesManager &e) {
		throw ExFuzzerModulesManager(e.getInfo());
	}
	this->outputtersModules.erase(outputterID);
}


//CFuzzerModulesManager::ExFuzzerModulesManager
//====================================================================================================
CFuzzerModulesManager::ExFuzzerModulesManager::ExModulesManager(const std::string &par_exInfo): exInfo(par_exInfo) {}

//====================================================================================================
std::string CModulesManager::ExModulesManager::getInfo() const {
	return std::string("[CFuzzerModulesManager EXCEPTION] ") + exInfo;
}
