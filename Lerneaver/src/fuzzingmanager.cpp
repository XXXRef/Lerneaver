#include "fuzzingmanager.hpp"

#include <iostream>
#include <algorithm>
#include <set>

#include "cfg.hpp"
#include "spdlog/sinks/basic_file_sink.h"

CFuzzingManager::CFuzzingManager() {
	// Logging stuff
	if (false==cfg::logging::flagLoggingEnabled) {
		this->pLogger->set_level(spdlog::level::off);
	}
	else {
		this->pLogger->set_level(spdlog::level::debug);
	}
}

//CFuzzerModulesContainer
//====================================================================================================
const std::string CFuzzerModulesContainer::fuzzerFactoryFunctionName = "getFuzzer"; // IFuzzer* getFuzzer()
const std::string CFuzzerModulesContainer::loggerFactoryFunctionName = "getLogger"; //ILogger* getLogger()
const std::string CFuzzerModulesContainer::outputterFactoryFunctionName = "getOutputter"; //IOutputter* getOutputter()

//====================================================================================================
std::shared_ptr<IFuzzer> CFuzzerModulesContainer::loadFuzzerFromModule(TYPE_FUZZERID fuzzerID, const TYPE_FILESYSTEMPATH& fuzzerLibPath) {
	this->fuzzersModules.insert({fuzzerID,fuzzerLibPath});
	this->modulesContainer.loadModule(fuzzerLibPath);
	auto pfnFuzzerFactoryFunction = (IFuzzer*(*)())this->modulesContainer[fuzzerLibPath]->getFunctionAddress(fuzzerFactoryFunctionName);
	return std::shared_ptr<IFuzzer>(pfnFuzzerFactoryFunction());
}

//====================================================================================================
void CFuzzerModulesContainer::unloadFuzzer(TYPE_FUZZERID fuzzerID) {
	this->modulesContainer.unloadModule(this->fuzzersModules[fuzzerID]);
	this->fuzzersModules.erase(fuzzerID);
}

//====================================================================================================
std::shared_ptr<ILogger> CFuzzerModulesContainer::loadLoggerFromModule(TYPE_LOGGERID loggerID, const TYPE_FILESYSTEMPATH& loggerLibPath) {
	this->loggersModules.insert({ loggerID, loggerLibPath });
	this->modulesContainer.loadModule(loggerLibPath);

	auto pfnLoggerFactoryFunction = (ILogger*(*)())this->modulesContainer[loggerLibPath]->getFunctionAddress(loggerFactoryFunctionName);
	return std::shared_ptr<ILogger>(pfnLoggerFactoryFunction());
}

//====================================================================================================
void CFuzzerModulesContainer::unloadLogger(TYPE_LOGGERID loggerID) {
	this->modulesContainer.unloadModule(this->loggersModules[loggerID]);
	this->loggersModules.erase(loggerID);
}

//====================================================================================================
std::shared_ptr<IOutputter> CFuzzerModulesContainer::loadOutputterFromModule(TYPE_OUTPUTTERID outputterID, const TYPE_FILESYSTEMPATH& outputterLibPath) {
	this->outputtersModules.insert({outputterID,outputterLibPath});
	this->modulesContainer.loadModule(outputterLibPath);

	auto pfnOutputterFactoryFunction = (IOutputter*(*)())this->modulesContainer[outputterLibPath]->getFunctionAddress(outputterFactoryFunctionName);
	return std::shared_ptr<IOutputter>(pfnOutputterFactoryFunction());
}

//====================================================================================================
void CFuzzerModulesContainer::unloadOutputter(TYPE_OUTPUTTERID outputterID) {
	this->modulesContainer.unloadModule(this->outputtersModules[outputterID]);
	this->outputtersModules.erase(outputterID);
}

//CFuzzingManager
//====================================================================================================

//Logging stuff
//====================================================================================================
void CFuzzingManager::enableLogging(const TYPE_FILESYSTEMPATH& logFilePath) {
	this->pLogger=spdlog::basic_logger_st("logger_FuzzingFramework", logFilePath));
}

//====================================================================================================
void disableLogging() {
	
}

//---- Fuzzer-related interface
//====================================================================================================
void CFuzzingManager::addFuzzer(TYPE_FUZZERID fuzzerID, const TYPE_FILESYSTEMPATH& fuzzerLibFilePath) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addFuzzer BEGIN. fuzzerID={} fuzzerLibFilePath = {}", convertWStr2Str(fuzzerID),convertWStr2Str(fuzzerLibFilePath));
	
	//Check if given fuzzerID already exists
	for (const auto& e: this->fuzzers) {
		if (e.first==fuzzerID) {
			this->pLogger->log(spdlog::level::err, "Given fuzzerID already in use");
			throw ExFuzzingManager("Given fuzzerID already in use");
		}
	}
	
	auto pFuzzerObj = this->fuzzerModulesContainer.loadFuzzerFromModule(fuzzerID, fuzzerLibFilePath);
	this->fuzzers.insert({fuzzerID,pFuzzerObj});
	this->links[fuzzerID] = { std::unique_ptr<std::mutex>(new std::mutex()),std::list<std::shared_ptr<IOutputter>>() };
	this->loggersLinks[fuzzerID] = { std::unique_ptr<std::mutex>(new std::mutex()),std::list<std::shared_ptr<ILogger>>() };
	//Add worker for fuzzer
	std::shared_ptr<CWorker> pWorkerObj(new CWorker(this));
	this->workers[fuzzerID] = { pWorkerObj, std::thread()};
	
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addFuzzer END");
}

//====================================================================================================
void CFuzzingManager::removeFuzzer(TYPE_FUZZERID fuzzerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeFuzzer BEGIN. fuzzerID={}", convertWStr2Str(fuzzerID));

	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false==flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}

	this->fuzzerModulesContainer.unloadFuzzer(fuzzerID);
	//TODO Stop/deinit fuzzer?
	this->fuzzers.erase(fuzzerID);
	this->links.erase(fuzzerID);
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeFuzzer END");
}

//====================================================================================================
void CFuzzingManager::initFuzzer(TYPE_FUZZERID fuzzerID, const TYPE_FILESYSTEMPATH& cfgFilePath){
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initFuzzer BEGIN. fuzzerID={} cfgFilePath={}", convertWStr2Str(fuzzerID), convertWStr2Str(cfgFilePath));
	
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}
	
	this->fuzzers[fuzzerID]->init(cfgFilePath);
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initFuzzer END");
}

//====================================================================================================
void CFuzzingManager::deinitFuzzer(TYPE_FUZZERID fuzzerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitFuzzer BEGIN. fuzzerID={}", convertWStr2Str(fuzzerID));
	
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}
	
	this->fuzzers[fuzzerID]->deinit();
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitFuzzer END");
}

//====================================================================================================
void CFuzzingManager::playFuzzer(TYPE_FUZZERID fuzzerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::playFuzzer BEGIN. fuzzerID={}", convertWStr2Str(fuzzerID));
	
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}
	
	//Start fuzzer thread
	//Check if fuzzer worker  already in workers
	bool flagFuzzerWorkerExists = false;
	for (const auto& e : this->workers) {
		if (e.first == fuzzerID) {
			flagFuzzerWorkerExists = true;
			break;
		}
	}
	if (false == flagFuzzerWorkerExists) {
		this->workers[fuzzerID].second = std::thread(std::ref(*(this->workers[fuzzerID].first.get())), fuzzerID);
	}
	else { // Appropriate worker exists
		if (EWorkerState::PLAYING == this->workers[fuzzerID].first->getState()) {//Check if fuzzer worker is in right state for playing
			this->pLogger->log(spdlog::level::err, "This fuzzer already running");
			throw ExFuzzingManager("This fuzzer already running");
		}
		std::shared_ptr<CWorker> pWorkerObj(new CWorker(this));
		this->workers[fuzzerID] = { pWorkerObj, std::thread(std::ref(*(pWorkerObj.get())), fuzzerID) };
	}

	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::playFuzzer END");
}

//====================================================================================================
void CFuzzingManager::stopFuzzer(TYPE_FUZZERID fuzzerID){
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::stopFuzzer BEGIN. fuzzerID={}", convertWStr2Str(fuzzerID));

	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}

	if (EWorkerState::STOPPED == this->workers[fuzzerID].first->getState()) {//Check if fuzzer worker is in right state for playing
		this->pLogger->log(spdlog::level::err, "This fuzzer already stopped");
		throw ExFuzzingManager("This fuzzer already stopped");
	}

	this->workers[fuzzerID].first->setState(CWorker::EWorkerState::STOPPED);
	this->workers[fuzzerID].second.join();
	//this->workers.erase(fuzzerID);
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::stopFuzzer END");
}

//---- Outputter-related interface
//====================================================================================================
void CFuzzingManager::addOutputter(TYPE_OUTPUTTERID outputterID, const TYPE_FILESYSTEMPATH& outputterLibFilePath) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addOutputter BEGIN. outputterID={} outputterLibFilePath={}", convertWStr2Str(outputterID), convertWStr2Str(outputterLibFilePath));
	
	//Check if given outputterID already exists
	for (const auto& e : this->outputters) {
		if (e.first == outputterID) {
			this->pLogger->log(spdlog::level::err, "Given outputterID already in use");
			throw ExFuzzingManager("Given outputterID already in use");
		}
	}
	
	auto pOutputterObj = this->fuzzerModulesContainer.loadOutputterFromModule(outputterID, outputterLibFilePath);
	this->outputters.insert({outputterID,pOutputterObj});
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addOutputter END");
}

//====================================================================================================
void CFuzzingManager::removeOutputter(TYPE_OUTPUTTERID outputterID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeOutputter BEGIN. outputterID={}", convertWStr2Str(outputterID));
	
	//Check if given outputterID exists
	bool flagOutputterIDExists = false;
	for (const auto& e : this->outputters) {
		if (e.first == outputterID) {
			flagOutputterIDExists = true;
			break;
		}
	}
	if (false == flagOutputterIDExists) {
		this->pLogger->log(spdlog::level::err, "Outputter with given outputterID doesnt exist");
		throw ExFuzzingManager("Outputter with given outputterID doesnt exist");
	}
	
	//Remove outputter from all links?
	auto pOutputterObj = this->outputters[outputterID];
	this->outputters.erase(outputterID);
	//Remove outputter from all links
	for (auto& e : this->links) {
		e.second.first->lock();
		auto iter = e.second.second.begin();
		while (iter!= e.second.second.end()) {
			if (*iter == pOutputterObj) {
				iter=e.second.second.erase(iter);
			}
			else {
				++iter;
			}
		}
		e.second.first->unlock();
	}
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeOutputter END");
}

//====================================================================================================
void CFuzzingManager::initOutputter(TYPE_OUTPUTTERID outputterID, const TYPE_FILESYSTEMPATH& cfgFilePath) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initOutputter BEGIN. outputterID={} cfgFilePath={}", convertWStr2Str(outputterID), convertWStr2Str(cfgFilePath));
	
	//Check if given outputterID exists
	bool flagOutputterIDExists = false;
	for (const auto& e : this->outputters) {
		if (e.first == outputterID) {
			flagOutputterIDExists = true;
			break;
		}
	}
	if (false == flagOutputterIDExists) {
		this->pLogger->log(spdlog::level::err, "Outputter with given outputterID doesnt exist");
		throw ExFuzzingManager("Outputter with given outputterID doesnt exist");
	}
	
	this->outputters[outputterID]->init(cfgFilePath);
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initOutputter END");
}

//====================================================================================================
void CFuzzingManager::deinitOutputter(TYPE_OUTPUTTERID outputterID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitOutputter BEGIN. outputterID={}", convertWStr2Str(outputterID));
	
	//Check if given outputterID exists
	bool flagOutputterIDExists = false;
	for (const auto& e : this->outputters) {
		if (e.first == outputterID) {
			flagOutputterIDExists = true;
			break;
		}
	}
	if (false == flagOutputterIDExists) {
		this->pLogger->log(spdlog::level::err, "Outputter with given outputterID doesnt exist");
		throw ExFuzzingManager("Outputter with given outputterID doesnt exist");
	}
	
	this->outputters[outputterID]->deinit();
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitOutputter END");
}

//---- Logger-related interface
//====================================================================================================
void CFuzzingManager::addLogger(TYPE_LOGGERID loggerID, const TYPE_FILESYSTEMPATH& loggerLibFilePath) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addLogger BEGIN. loggerID={} loggerLibFilePath={}", convertWStr2Str(loggerID), convertWStr2Str(loggerLibFilePath));

	//Check if given loggerID already exists
	for (const auto& e : this->loggers) {
		if (e.first == loggerID) {
			this->pLogger->log(spdlog::level::err, "Given loggerID already in use");
			throw ExFuzzingManager("Given loggerID already in use");
		}
	}

	auto pLoggerObj = this->fuzzerModulesContainer.loadLoggerFromModule(loggerID, loggerLibFilePath);
	this->loggers.insert({ loggerID,pLoggerObj });
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addLogger END");
}

//====================================================================================================
void CFuzzingManager::removeLogger(TYPE_LOGGERID loggerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeLogger BEGIN. loggerID={}", convertWStr2Str(loggerID));

	//Check if given loggerID exists
	bool flagLoggerIDExists = false;
	for (const auto& e : this->loggers) {
		if (e.first == loggerID) {
			flagLoggerIDExists = true;
			break;
		}
	}
	if (false == flagLoggerIDExists) {
		this->pLogger->log(spdlog::level::err, "Logger with given loggerID doesnt exist");
		throw ExFuzzingManager("Logger with given loggerID doesnt exist");
	}

	//Remove logger from all links?
	auto pLoggerObj = this->loggers[loggerID];
	this->loggers.erase(loggerID);
	//Remove logger from all links
	for (auto& e : this->loggersLinks) {
		e.second.first->lock();
		auto iter = e.second.second.begin();
		while (iter != e.second.second.end()) {
			if (*iter == pLoggerObj) {
				iter = e.second.second.erase(iter);
			}
			else {
				++iter;
			}
		}
		e.second.first->unlock();
	}
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeLogger END");
}

//====================================================================================================
void CFuzzingManager::initLogger(TYPE_LOGGERID loggerID, const TYPE_FILESYSTEMPATH& cfgFilePath) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initLogger BEGIN. loggerID={} cfgFilePath={}", convertWStr2Str(loggerID), convertWStr2Str(cfgFilePath));

	//Check if given loggerID exists
	bool flagLoggerIDExists = false;
	for (const auto& e : this->loggers) {
		if (e.first == loggerID) {
			flagLoggerIDExists = true;
			break;
		}
	}
	if (false == flagLoggerIDExists) {
		this->pLogger->log(spdlog::level::err, "Logger with given loggerID doesnt exist");
		throw ExFuzzingManager("Logger with given loggerID doesnt exist");
	}

	this->loggers[loggerID]->init(cfgFilePath);
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initLogger END");
}

//====================================================================================================
void CFuzzingManager::deinitLogger(TYPE_LOGGERID loggerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitLogger BEGIN. loggerID={}", convertWStr2Str(loggerID));

	//Check if given loggerID exists
	bool flagLoggerIDExists = false;
	for (const auto& e : this->loggers) {
		if (e.first == loggerID) {
			flagLoggerIDExists = true;
			break;
		}
	}
	if (false == flagLoggerIDExists) {
		this->pLogger->log(spdlog::level::err, "Logger with given loggerID doesnt exist");
		throw ExFuzzingManager("Logger with given loggerID doesnt exist");
	}

	this->loggers[loggerID]->deinit();
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitLogger END");
}

//---- Loggers-links-related interface
//====================================================================================================
void CFuzzingManager::setFuzzerLoggers(TYPE_FUZZERID fuzzerID, const std::list<TYPE_LOGGERID>& par_loggers) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerLoggers BEGIN. fuzzerID={}", convertWStr2Str(fuzzerID));

	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}

	//Check if given loggersIDs exist
	bool flagLoggerIDExists;
	for (const auto& e1 : par_loggers) {
		flagLoggerIDExists = false;
		for (const auto& e2 : this->loggers) {
			if (e2.first == e1) {
				flagLoggerIDExists = true;
				break;
			}
		}
		if (false == flagLoggerIDExists) {
			this->pLogger->log(spdlog::level::err, "One of loggerIDs doesnt exist");
			throw ExFuzzingManager("One of loggerIDs doesnt exist");
		}
	}

	//Erase prev links
	this->loggersLinks[fuzzerID].first->lock();
	this->loggersLinks.erase(fuzzerID);
	//this->links[fuzzerID] = std::pair<std::unique_ptr<std::mutex>, std::list<std::shared_ptr<IOutputter>>>(std::unique_ptr<std::mutex>(new std::mutex()), std::list<std::shared_ptr<IOutputter>>());
	this->loggersLinks[fuzzerID] = { std::unique_ptr<std::mutex>(new std::mutex()), std::list<std::shared_ptr<ILogger>>() };

	//Add new links
	for (const auto& e : this->loggers) {
		this->loggersLinks[fuzzerID].second.push_back(this->loggers[e.first]);
	}
	this->loggersLinks[fuzzerID].first->unlock();

	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerLoggers END");
}

//====================================================================================================
void CFuzzingManager::addFuzzerLogger(TYPE_FUZZERID fuzzerID, TYPE_LOGGERID loggerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addFuzzerLogger BEGIN. fuzzerID={} loggerID={}", convertWStr2Str(fuzzerID), convertWStr2Str(loggerID));

	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}
	//Check if given loggerID exists
	bool flagLoggerIDExists = false;
	for (const auto& e : this->loggers) {
		if (e.first == loggerID) {
			flagLoggerIDExists = true;
			break;
		}
	}
	if (false == flagLoggerIDExists) {
		this->pLogger->log(spdlog::level::err, "Logger with given loggerID doesnt exist");
		throw ExFuzzingManager("Logger with given loggerID doesnt exist");
	}

	auto pLoggerObj = this->loggers[loggerID];
	this->loggersLinks[fuzzerID].first->lock();
	this->loggersLinks[fuzzerID].second.push_back(pLoggerObj);
	this->loggersLinks[fuzzerID].first->unlock();
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerLoggers END");
}

//====================================================================================================
void CFuzzingManager::removeFuzzerLogger(TYPE_FUZZERID fuzzerID, TYPE_LOGGERID loggerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeFuzzerLogger BEGIN. fuzzerID={} loggerID={}", convertWStr2Str(fuzzerID), convertWStr2Str(loggerID));

	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}
	//Check if given loggerID exists
	bool flagLoggerIDExists = false;
	for (const auto& e : this->loggers) {
		if (e.first == loggerID) {
			flagLoggerIDExists = true;
			break;
		}
	}
	if (false == flagLoggerIDExists) {
		this->pLogger->log(spdlog::level::err, "Logger with given loggerID doesnt exist");
		throw ExFuzzingManager("Logger with given loggerID doesnt exist");
	}

	auto pLoggerObj = this->loggers[loggerID];

	this->loggersLinks[fuzzerID].first->lock();
	auto searchResultIter = this->loggersLinks[fuzzerID].second.begin();
	do {
		searchResultIter = std::find(searchResultIter, this->loggersLinks[fuzzerID].second.end(), pLoggerObj);
		if (searchResultIter == this->loggersLinks[fuzzerID].second.end()) {
			break;
		}
		searchResultIter = this->loggersLinks[fuzzerID].second.erase(searchResultIter);
	} while (true);

	this->loggersLinks[fuzzerID].first->unlock();
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeFuzzerLogger END");
}

//---- Links-related interface
//====================================================================================================
void CFuzzingManager::setFuzzerOutputters(TYPE_FUZZERID fuzzerID, const std::list<TYPE_OUTPUTTERID>& par_outputters) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerOutputters BEGIN. fuzzerID={}", convertWStr2Str(fuzzerID));
	
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}

	//Check if given outputterIDs exist
	bool flagOutputterIDExists;
	for(const auto& e1: par_outputters){
		flagOutputterIDExists = false;
		for (const auto& e2 : this->outputters) {
			if (e2.first == e1) {
				flagOutputterIDExists = true;
				break;
			}
		}
		if (false == flagOutputterIDExists) {
			this->pLogger->log(spdlog::level::err, "One of outputterIDs doesnt exist");
			throw ExFuzzingManager("One of outputterIDs doesnt exist");
		}
	}

	//Erase prev links
	this->links[fuzzerID].first->lock();
	this->links.erase(fuzzerID);
	//this->links[fuzzerID] = std::pair<std::unique_ptr<std::mutex>, std::list<std::shared_ptr<IOutputter>>>(std::unique_ptr<std::mutex>(new std::mutex()), std::list<std::shared_ptr<IOutputter>>());
	this->links[fuzzerID] = { std::unique_ptr<std::mutex>(new std::mutex()), std::list<std::shared_ptr<IOutputter>>() };

	//Add new links
	for (const auto& e: this->outputters) {
		this->links[fuzzerID].second.push_back(this->outputters[e.first]);
	}
	this->links[fuzzerID].first->unlock();

	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerOutputters END");
}

//====================================================================================================
void CFuzzingManager::addFuzzerOutputter(TYPE_FUZZERID fuzzerID, TYPE_OUTPUTTERID outputterID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addFuzzerOutputter BEGIN. fuzzerID={} outputterID={}", convertWStr2Str(fuzzerID), convertWStr2Str(outputterID));
	
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}
	//Check if given outputterID exists
	bool flagOutputterIDExists = false;
	for (const auto& e : this->outputters) {
		if (e.first == outputterID) {
			flagOutputterIDExists = true;
			break;
		}
	}
	if (false == flagOutputterIDExists) {
		this->pLogger->log(spdlog::level::err, "Outputter with given outputterID doesnt exist");
		throw ExFuzzingManager("Outputter with given outputterID doesnt exist");
	}
	
	auto pOutputterObj = this->outputters[outputterID];
	this->links[fuzzerID].first->lock();
	this->links[fuzzerID].second.push_back(pOutputterObj);
	this->links[fuzzerID].first->unlock();
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerOutputters END");
}

//====================================================================================================
void CFuzzingManager::removeFuzzerOutputter(TYPE_FUZZERID fuzzerID, TYPE_OUTPUTTERID outputterID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeFuzzerOutputter BEGIN. fuzzerID={} outputterID={}", convertWStr2Str(fuzzerID), convertWStr2Str(outputterID));
	
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}
	//Check if given outputterID exists
	bool flagOutputterIDExists = false;
	for (const auto& e : this->outputters) {
		if (e.first == outputterID) {
			flagOutputterIDExists = true;
			break;
		}
	}
	if (false == flagOutputterIDExists) {
		this->pLogger->log(spdlog::level::err, "Outputter with given outputterID doesnt exist");
		throw ExFuzzingManager("Outputter with given outputterID doesnt exist");
	}
	
	auto pOutputterObj = this->outputters[outputterID];
	
	this->links[fuzzerID].first->lock();
	auto searchResultIter= this->links[fuzzerID].second.begin();
	do{
		searchResultIter = std::find(searchResultIter, this->links[fuzzerID].second.end(), pOutputterObj);
		if (searchResultIter == this->links[fuzzerID].second.end()) {
			break;
		}
		searchResultIter=this->links[fuzzerID].second.erase(searchResultIter);
	} while (true);
	
	this->links[fuzzerID].first->unlock();
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeFuzzerOutputter END");
}

//====================================================================================================
std::tuple<CFuzzingManager::EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID>> CFuzzingManager::getFuzzerState(TYPE_FUZZERID fuzzerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::getFuzzerState BEGIN. fuzzerID={}", convertWStr2Str(fuzzerID));

	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto& e : this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}
	
	//Get fuzzer worker state
	auto fuzzerWorkerState = this->workers[fuzzerID].first->getState();

	//Get fuzzer loggers
	std::vector<TYPE_LOGGERID> fuzzerLoggers;
	this->loggersLinks[fuzzerID].first->lock();
	for (const auto& e : this->loggersLinks[fuzzerID].second) {
		for (auto iter = this->loggers.begin(); iter != this->loggers.end(); ++iter) {
			if (e == iter->second) {
				fuzzerLoggers.push_back(iter->first);
			}
		}
	}
	this->loggersLinks[fuzzerID].first->unlock();

	//Get fuzzer outputters
	std::vector<TYPE_OUTPUTTERID> fuzzerOutputters;
	this->links[fuzzerID].first->lock();
	for (const auto& e: this->links[fuzzerID].second) {
		for (auto iter = this->outputters.begin(); iter != this->outputters.end(); ++iter) {
			if (e == iter->second) {
				fuzzerOutputters.push_back(iter->first);
			}
		}
	}
	this->links[fuzzerID].first->unlock();

	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::getFuzzerState END");

	return std::tuple<CFuzzingManager::EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID>>(fuzzerWorkerState, fuzzerLoggers, fuzzerOutputters);
}

//====================================================================================================
std::vector<std::tuple<TYPE_FUZZERID, CFuzzingManager::EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID> >> CFuzzingManager::getState() {
	std::vector<std::tuple<TYPE_FUZZERID, CFuzzingManager::EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID> >> resultVec;

	//1. Get set of fuzzer IDs
	std::set<TYPE_FUZZERID> fuzzersIDs;
	for (auto iter = this->fuzzers.begin(); iter != this->fuzzers.end(); ++iter) {
		fuzzersIDs.insert(iter->first);
	}

	//2. Invoke getFuzzerState for each fuzzerID
	for (auto iter = fuzzersIDs.begin(); iter != fuzzersIDs.end();++iter) {
		auto fuzzerState = this->getFuzzerState(*iter);
		resultVec.push_back(std::tie(*iter, std::get<0>(fuzzerState), std::get<1>(fuzzerState), std::get<2>(fuzzerState))); // {0:fuzzerWorkerState, 1:fuzzerLoggers, 2::fuzzerOutputters}
	}

	return resultVec;
}



//CWorker
//====================================================================================================
CFuzzingManager::CWorker::CWorker(CFuzzingManager* par_pFuzzingManager): pFuzzingManager(par_pFuzzingManager) {}

void CFuzzingManager::CWorker::operator()(TYPE_FUZZERID fuzzerID) {
	this->state.set(EWorkerState::PLAYING);

	std::shared_ptr<IFuzzer> pFuzzer = pFuzzingManager->fuzzers[fuzzerID];
	
	while(true){
	//TODO Add time delaying
	//1. Get buffer from fuzzer object
		auto fuzzerOutputDataBlob = pFuzzer->gen();

	//2. Pass fuzzer data to loggers
		//Produce metadata
		//Get fuzzer name
		auto fuzzerIDBytesSize = fuzzerID.size() * sizeof(fuzzerID[0]);
		ByteBlob metadataBlob(fuzzerIDBytesSize);
		std::memcpy(&metadataBlob[0], &fuzzerID[0], fuzzerIDBytesSize);

		pFuzzingManager->loggersLinks[fuzzerID].first->lock();
		for (const auto& pLoggerObj : pFuzzingManager->loggersLinks[fuzzerID].second) {
			pLoggerObj->log(fuzzerOutputDataBlob, metadataBlob);
		}
		pFuzzingManager->loggersLinks[fuzzerID].first->unlock();

	//3. Pass fuzzer buffer to outputters?
		pFuzzingManager->links[fuzzerID].first->lock();
		
		for (const auto& pOutputterObj: pFuzzingManager->links[fuzzerID].second) {
			pOutputterObj->output(fuzzerOutputDataBlob);
		}
		
		pFuzzingManager->links[fuzzerID].first->unlock();

		//Check state
		auto stateVal = this->state.get();
		if ((EWorkerState::STOPPED==stateVal) || (EWorkerState::INTERRUPTED==stateVal)) {
			// Interrupt worker loop
			break;
		}
	}
}

//====================================================================================================
CFuzzingManager::CWorker::EWorkerState CFuzzingManager::CWorker::getState() {
	return this->state.get();
}

//====================================================================================================
void CFuzzingManager::CWorker::setState(const EWorkerState& par_state) {
	return this->state.set(par_state);
}

//====================================================================================================
CFuzzingManager::ExFuzzingManager::ExFuzzingManager(const std::string& par_msg): ExException(par_msg)  {}