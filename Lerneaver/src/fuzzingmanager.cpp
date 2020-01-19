#include "fuzzingmanager.hpp"

#include <iostream>
#include <algorithm>
#include <set>
#include <cstdlib>
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/null_sink.h"

//CFuzzingManager
//====================================================================================================
CFuzzingManager::CFuzzingManager() {
	//Set logger
	std::srand(std::time(nullptr));
	this->fuzzingManagerID = std::rand();
	this->pLogger = spdlog::create<spdlog::sinks::null_sink_st>(std::string("logger_Lerneaver_") + std::to_string(this->fuzzingManagerID));
}

CFuzzingManager::~CFuzzingManager() {
	//Remove all remaining fuzzers	
	//Get all fuzzers IDs
	std::vector<decltype(this->fuzzers)::key_type> fuzzersIDs(this->fuzzers.size());
	decltype(this->fuzzers)::size_type i = 0;
	for (auto iter = this->fuzzers.cbegin(); iter != this->fuzzers.cend(); ++iter) fuzzersIDs[i++] = iter->first;
	for (const auto &e: fuzzersIDs) {
		//Stop fuzzer if its playing
		if (EWorkerState::PLAYING == this->workers[e].first->getState()) {
			this->workers[e].first->setState(EWorkerState::STOPPED);
			this->workers[e].second.join();
		}
		this->workers.erase(e);
		this->fuzzers.erase(e);
		this->links.erase(e);
		this->loggersLinks.erase(e);
		this->fuzzerModulesContainer.unloadFuzzer(e);
	}
	//Cleanup logging
	spdlog::drop(std::string("logger_Lerneaver_") + std::to_string(this->fuzzingManagerID));
}

//Logging stuff
//====================================================================================================
void CFuzzingManager::enableLogging(const config::platform::TYPE_FILESYSTEMPATH &logFilePath) {
	spdlog::drop(std::string("logger_Lerneaver_") + std::to_string(this->fuzzingManagerID));
	this->pLogger = spdlog::basic_logger_st(std::string("logger_Lerneaver_") + std::to_string(this->fuzzingManagerID), logFilePath);
	this->pLogger->set_level(spdlog::level::debug);
}

//====================================================================================================
void CFuzzingManager::disableLogging() {
	spdlog::drop(std::string("logger_Lerneaver_") + std::to_string(this->fuzzingManagerID));
	this->pLogger = spdlog::create<spdlog::sinks::null_sink_st>(std::string("logger_Lerneaver_") + std::to_string(this->fuzzingManagerID));
	//this->pLogger->set_level(spdlog::level::off);
}

//Fuzzer-related interface
//====================================================================================================
void CFuzzingManager::addFuzzer(const TYPE_FUZZERID &fuzzerID, const TYPE_MODID &fuzzerModID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addFuzzer BEGIN. fuzzerID={} fuzzerLibFilePath = {}", fuzzerID, fuzzerModID);
	
	//Check if given fuzzerID already exists
	for (const auto &e: this->fuzzers) {
		if (e.first==fuzzerID) { //WARNING TYPE_FUZZERID must be comparable
			this->pLogger->log(spdlog::level::err, "Given fuzzerID already in use");
			throw ExFuzzingManager("Given fuzzerID already in use");
		}
	}
	std::shared_ptr<IFuzzer> pFuzzerObj;
	try{
		pFuzzerObj = this->fuzzerModulesContainer.loadFuzzerFromModule(fuzzerID, fuzzerModID);
	}
	catch (CFuzzerModulesManager::ExFuzzerModulesManager &e) {
		this->pLogger->log(spdlog::level::err, std::string("Failed to load fuzzer module: ")+e.getInfo());
		throw ExFuzzingManager(e.getInfo());
	}
	//Add new fuzzer into data structures
	this->fuzzers.insert({fuzzerID,pFuzzerObj });
	this->links[fuzzerID] = { std::unique_ptr<std::mutex>(new std::mutex()),std::list<std::shared_ptr<IOutputter>>() };//TIP std::make_unique introdused in latter standards
	this->loggersLinks[fuzzerID] = { std::unique_ptr<std::mutex>(new std::mutex()),std::list<std::shared_ptr<ILogger>>() };
	//Add worker for fuzzer
	std::shared_ptr<CWorker> pWorkerObj(new CWorker(this));
	this->workers[fuzzerID] = { pWorkerObj, std::thread()};
	
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addFuzzer END");
}

//====================================================================================================
void CFuzzingManager::removeFuzzer(const TYPE_FUZZERID &fuzzerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeFuzzer BEGIN. fuzzerID={}", fuzzerID);

	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto &e: this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false==flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}

	//Stop fuzzer
	//TIP stop not deinit or something else. User cant do it himself by appropriate command. If fuzzer is playing while its removing all that needed is to make sure fuzzer correctly removed
	if (EWorkerState::PLAYING == this->workers[fuzzerID].first->getState()) {
		this->pLogger->log(spdlog::level::err, "Attempted to remove playing fuzzer. Stop it before removing");
		throw ExFuzzingManager("Attempted to remove playing fuzzer. Stop it before removing");
	}
	this->workers.erase(fuzzerID);
	this->fuzzers.erase(fuzzerID);
	this->links.erase(fuzzerID);
	this->loggersLinks.erase(fuzzerID);

	try {
		this->fuzzerModulesContainer.unloadFuzzer(fuzzerID);
	}
	catch (CFuzzerModulesManager::ExFuzzerModulesManager & e) {
		this->pLogger->log(spdlog::level::err, std::string("Failed to unload fuzzer module: ") + e.getInfo());
		throw ExFuzzingManager(e.getInfo());
	}

	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeFuzzer END");
}

//====================================================================================================
void CFuzzingManager::initFuzzer(const TYPE_FUZZERID &fuzzerID, const TYPE_CONFIGID &cfgFileID){
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initFuzzer BEGIN. fuzzerID={} cfgFilePath={}", fuzzerID, cfgFileID);
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto &e: this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}
	//Check if fuzzer is playing
	if (EWorkerState::PLAYING == this->workers[fuzzerID].first->getState()) {
		this->pLogger->log(spdlog::level::err, "Attempted to init playing fuzzer. Stop it before initializing");
		throw ExFuzzingManager("Attempted to init playing fuzzer. Stop it before initializing");
	}
	//Init fuzzer
	this->fuzzers[fuzzerID]->init(cfgFileID);
	this->workers[fuzzerID].first->setState(EWorkerState::INITIALIZED);
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initFuzzer END");
}

//====================================================================================================
void CFuzzingManager::deinitFuzzer(const TYPE_FUZZERID &fuzzerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitFuzzer BEGIN. fuzzerID={}", fuzzerID);
	
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;;
	for (const auto &e: this->fuzzers) {
		if (e.first == fuzzerID) {
			flagFuzzerIDExists = true;
			break;
		}
	}
	if (false == flagFuzzerIDExists) {
		this->pLogger->log(spdlog::level::err, "Fuzzer with given fuzzerID doesnt exist");
		throw ExFuzzingManager("Fuzzer with given fuzzerID doesnt exist");
	}
	//Check if fuzzer is playing
	if (EWorkerState::PLAYING == this->workers[fuzzerID].first->getState()) {
		this->pLogger->log(spdlog::level::err, "Attempted to deinit playing fuzzer. Stop it before deinitializing");
		throw ExFuzzingManager("Attempted to deinit playing fuzzer. Stop it before deinitializing");
	}
	//Deinit fuzzer
	this->fuzzers[fuzzerID]->deinit();
	this->workers[fuzzerID].first->setState(EWorkerState::DEINITIALIZED);
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitFuzzer END");
}

//====================================================================================================
void CFuzzingManager::playFuzzer(const TYPE_FUZZERID &fuzzerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::playFuzzer BEGIN. fuzzerID={}", fuzzerID);
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto &e: this->fuzzers) {
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
	for (const auto &e : this->workers) {
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
void CFuzzingManager::stopFuzzer(const TYPE_FUZZERID &fuzzerID){
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::stopFuzzer BEGIN. fuzzerID={}", fuzzerID);
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto &e: this->fuzzers) {
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

//Outputter-related interface
//====================================================================================================
void CFuzzingManager::addOutputter(const TYPE_OUTPUTTERID &outputterID, const TYPE_MODID &outputterModID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addOutputter BEGIN. outputterID={} outputterLibFilePath={}", outputterID, outputterModID);
	
	//Check if given outputterID already exists
	for (const auto &e: this->outputters) {
		if (e.first == outputterID) {
			this->pLogger->log(spdlog::level::err, "Given outputterID already in use");
			throw ExFuzzingManager("Given outputterID already in use");
		}
	}
	std::shared_ptr<IOutputter> pOutputterObj;
	try{
		pOutputterObj = this->fuzzerModulesContainer.loadOutputterFromModule(outputterID, outputterModID);
	}
	catch (CFuzzerModulesManager::ExFuzzerModulesManager &e) {
		this->pLogger->log(spdlog::level::err, std::string("Failed to load outputter module: ") + e.getInfo());
		throw ExFuzzingManager(e.getInfo());
	}
	this->outputters.insert({outputterID,pOutputterObj});
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addOutputter END");
}

//====================================================================================================
void CFuzzingManager::removeOutputter(const TYPE_OUTPUTTERID &outputterID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeOutputter BEGIN. outputterID={}", outputterID);
	
	//Check if given outputterID exists
	bool flagOutputterIDExists = false;
	for (const auto &e: this->outputters) {
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
	for (auto &e: this->links) {
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
void CFuzzingManager::initOutputter(const TYPE_OUTPUTTERID &outputterID, const TYPE_CONFIGID &cfgFileID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initOutputter BEGIN. outputterID={} cfgFilePath={}", outputterID, cfgFileID);
	//Check if given outputterID exists
	bool flagOutputterIDExists = false;
	for (const auto &e: this->outputters) {
		if (e.first == outputterID) {
			flagOutputterIDExists = true;
			break;
		}
	}
	if (false == flagOutputterIDExists) {
		this->pLogger->log(spdlog::level::err, "Outputter with given outputterID doesnt exist");
		throw ExFuzzingManager("Outputter with given outputterID doesnt exist");
	}
	//Init outputter
	this->outputters[outputterID]->init(cfgFileID);
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initOutputter END");
}

//====================================================================================================
void CFuzzingManager::deinitOutputter(const TYPE_OUTPUTTERID &outputterID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitOutputter BEGIN. outputterID={}", outputterID);
	
	//Check if given outputterID exists
	bool flagOutputterIDExists = false;
	for (const auto &e: this->outputters) {
		if (e.first == outputterID) {
			flagOutputterIDExists = true;
			break;
		}
	}
	if (false == flagOutputterIDExists) {
		this->pLogger->log(spdlog::level::err, "Outputter with given outputterID doesnt exist");
		throw ExFuzzingManager("Outputter with given outputterID doesnt exist");
	}
	//Deinit outputter
	this->outputters[outputterID]->deinit();
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitOutputter END");
}

//---- Logger-related interface
//====================================================================================================
void CFuzzingManager::addLogger(const TYPE_LOGGERID &loggerID, const TYPE_MODID &loggerModID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addLogger BEGIN. loggerID={} loggerLibFilePath={}", loggerID, loggerModID);
	//Check if given loggerID already exists
	for (const auto &e: this->loggers) {
		if (e.first == loggerID) {
			this->pLogger->log(spdlog::level::err, "Given loggerID already in use");
			throw ExFuzzingManager("Given loggerID already in use");
		}
	}
	std::shared_ptr<ILogger> pLoggerObj;
	try {
		pLoggerObj = this->fuzzerModulesContainer.loadLoggerFromModule(loggerID, loggerModID);
	}
	catch (CFuzzerModulesManager::ExFuzzerModulesManager &e) {
		this->pLogger->log(spdlog::level::err, std::string("Failed to load logger module: ") + e.getInfo());
		throw ExFuzzingManager(e.getInfo());
	}
	this->loggers.insert({ loggerID,pLoggerObj });
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addLogger END");
}

//====================================================================================================
void CFuzzingManager::removeLogger(const TYPE_LOGGERID &loggerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeLogger BEGIN. loggerID={}", loggerID);

	//Check if given loggerID exists
	bool flagLoggerIDExists = false;
	for (const auto &e: this->loggers) {
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
	for (auto &e: this->loggersLinks) {
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
void CFuzzingManager::initLogger(const TYPE_LOGGERID &loggerID, const TYPE_CONFIGID &cfgFileID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initLogger BEGIN. loggerID={} cfgFilePath={}", loggerID, cfgFileID);

	//Check if given loggerID exists
	bool flagLoggerIDExists = false;
	for (const auto &e: this->loggers) {
		if (e.first == loggerID) {
			flagLoggerIDExists = true;
			break;
		}
	}
	if (false == flagLoggerIDExists) {
		this->pLogger->log(spdlog::level::err, "Logger with given loggerID doesnt exist");
		throw ExFuzzingManager("Logger with given loggerID doesnt exist");
	}
	//Init logger
	this->loggers[loggerID]->init(cfgFileID);
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::initLogger END");
}

//====================================================================================================
void CFuzzingManager::deinitLogger(const TYPE_LOGGERID &loggerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitLogger BEGIN. loggerID={}", loggerID);

	//Check if given loggerID exists
	bool flagLoggerIDExists = false;
	for (const auto &e: this->loggers) {
		if (e.first == loggerID) {
			flagLoggerIDExists = true;
			break;
		}
	}
	if (false == flagLoggerIDExists) {
		this->pLogger->log(spdlog::level::err, "Logger with given loggerID doesnt exist");
		throw ExFuzzingManager("Logger with given loggerID doesnt exist");
	}
	//Deinit logger
	this->loggers[loggerID]->deinit();
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::deinitLogger END");
}

//---- Loggers-links-related interface
//====================================================================================================
void CFuzzingManager::setFuzzerLoggers(const TYPE_FUZZERID &fuzzerID, const std::list<TYPE_LOGGERID> &par_loggers) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerLoggers BEGIN. fuzzerID={}", fuzzerID);

	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto &e: this->fuzzers) {
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
	for (const auto &e1: par_loggers) {
		flagLoggerIDExists = false;
		for (const auto &e2: this->loggers) {
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
	for (const auto &e: this->loggers) {
		this->loggersLinks[fuzzerID].second.push_back(this->loggers[e.first]);
	}
	this->loggersLinks[fuzzerID].first->unlock();

	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerLoggers END");
}

//====================================================================================================
void CFuzzingManager::addFuzzerLogger(const TYPE_FUZZERID &fuzzerID, const TYPE_LOGGERID &loggerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addFuzzerLogger BEGIN. fuzzerID={} loggerID={}", fuzzerID, loggerID);

	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto &e: this->fuzzers) {
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
	for (const auto &e: this->loggers) {
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
	//Check if logger already linked to fuzzer
	this->loggersLinks[fuzzerID].first->lock();
	if (std::find(this->loggersLinks[fuzzerID].second.cbegin(), this->loggersLinks[fuzzerID].second.cend(), pLoggerObj) != this->loggersLinks[fuzzerID].second.cend()) { //TODO This is bullshit to compare find by shared ptrs. loggersLinks must contain linked loggers IDs
		this->loggersLinks[fuzzerID].first->unlock();
		this->pLogger->log(spdlog::level::err, "Logger with given loggerID is already linked to this fuzzer");
		throw ExFuzzingManager("Logger with given loggerID is already linked to this fuzzer");
	}
	this->loggersLinks[fuzzerID].second.push_back(pLoggerObj);
	this->loggersLinks[fuzzerID].first->unlock();
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerLoggers END");
}

//====================================================================================================
void CFuzzingManager::removeFuzzerLogger(const TYPE_FUZZERID &fuzzerID, const TYPE_LOGGERID &loggerID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeFuzzerLogger BEGIN. fuzzerID={} loggerID={}", fuzzerID, loggerID);

	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto &e: this->fuzzers) {
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
	for (const auto &e: this->loggers) {
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
void CFuzzingManager::setFuzzerOutputters(const TYPE_FUZZERID &fuzzerID, const std::list<TYPE_OUTPUTTERID> &par_outputters) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerOutputters BEGIN. fuzzerID={}", fuzzerID);
	
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto &e: this->fuzzers) {
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
	for(const auto &e1: par_outputters){
		flagOutputterIDExists = false;
		for (const auto &e2: this->outputters) {
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
	for (const auto &e: this->outputters) {
		this->links[fuzzerID].second.push_back(this->outputters[e.first]);
	}
	this->links[fuzzerID].first->unlock();

	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerOutputters END");
}

//====================================================================================================
void CFuzzingManager::addFuzzerOutputter(const TYPE_FUZZERID &fuzzerID, const TYPE_OUTPUTTERID &outputterID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::addFuzzerOutputter BEGIN. fuzzerID={} outputterID={}", fuzzerID, outputterID);
	
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto &e: this->fuzzers) {
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
	for (const auto &e: this->outputters) {
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
	//Check if outputter already linked to fuzzer
	this->links[fuzzerID].first->lock();
	if (std::find(this->links[fuzzerID].second.cbegin(), this->links[fuzzerID].second.cend(), pOutputterObj) != this->links[fuzzerID].second.cend()) { //TODO This is bullshit to compare find by shared ptrs. links must contain linked loggers IDs
		this->links[fuzzerID].first->unlock();
		this->pLogger->log(spdlog::level::err, "Outputter with given outputterID is already linked to this fuzzer");
		throw ExFuzzingManager("Outputter with given outputterID is already linked to this fuzzer");
	}
	this->links[fuzzerID].second.push_back(pOutputterObj);
	this->links[fuzzerID].first->unlock();
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::setFuzzerOutputters END");
}

//====================================================================================================
void CFuzzingManager::removeFuzzerOutputter(const TYPE_FUZZERID &fuzzerID, const TYPE_OUTPUTTERID &outputterID) {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::removeFuzzerOutputter BEGIN. fuzzerID={} outputterID={}", fuzzerID, outputterID);
	
	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto &e: this->fuzzers) {
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
	for (const auto &e: this->outputters) {
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
auto  CFuzzingManager::getFuzzerState(const TYPE_FUZZERID &fuzzerID) -> std::tuple<CFuzzingManager::EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID>> {
	this->pLogger->log(spdlog::level::debug, "CFuzzingManager::getFuzzerState BEGIN. fuzzerID={}", fuzzerID);

	//Check if given fuzzerID exists
	bool flagFuzzerIDExists = false;
	for (const auto &e: this->fuzzers) {
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
	for (const auto &e: this->loggersLinks[fuzzerID].second) {
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
	for (const auto &e: this->links[fuzzerID].second) {
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
auto CFuzzingManager::getState() -> std::vector<std::tuple<TYPE_FUZZERID, CFuzzingManager::EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID> >> {
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
CFuzzingManager::CWorker::CWorker(CFuzzingManager *par_pFuzzingManager): pFuzzingManager(par_pFuzzingManager) {}

void CFuzzingManager::CWorker::operator()(const TYPE_FUZZERID &fuzzerID) {
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

//CFuzzingManager::CWorker
//====================================================================================================
CFuzzingManager::CWorker::EWorkerState CFuzzingManager::CWorker::getState() {
	return this->state.get();
}

//====================================================================================================
void CFuzzingManager::CWorker::setState(const EWorkerState& par_state) {
	return this->state.set(par_state);
}
