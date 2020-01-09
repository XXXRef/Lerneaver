#ifndef HEADER_FUZZINGMANAGER
#define HEADER_FUZZINGMANAGER

#include <string>
#include <memory>
#include <map>
#include <list>
#include <mutex>
#include <vector>
#include <thread>
#include <string>

#include "types.hpp"
#include "utils.hpp"
#include "i_fuzzer.hpp"
#include "i_logger.hpp"
#include "i_outputter.hpp"

#include "ModulesStuff/modulescontainer.hpp"

#include "spdlog/spdlog.h"

//====================================================================================================
using TYPE_FUZZERID = std::wstring; //if changing this need to change user utils
using TYPE_LOGGERID = std::wstring;
using TYPE_OUTPUTTERID = std::wstring;

//====================================================================================================
/**
	\brief Class that manages fuzzers/outputters modules loading/unloading by their IDs
*/
class CFuzzerModulesContainer {
	CModulesContainer modulesContainer;
	std::map<TYPE_FUZZERID, TYPE_FILESYSTEMPATH> fuzzersModules;
	std::map<TYPE_LOGGERID, TYPE_FILESYSTEMPATH> loggersModules;
	std::map<TYPE_OUTPUTTERID, TYPE_FILESYSTEMPATH> outputtersModules;

	static const std::string fuzzerFactoryFunctionName;
	static const std::string outputterFactoryFunctionName;
	static const std::string loggerFactoryFunctionName;

public:
	std::shared_ptr<IFuzzer> loadFuzzerFromModule(TYPE_FUZZERID fuzzerID, const TYPE_FILESYSTEMPATH& fuzzerLibPath);
	void unloadFuzzer(TYPE_FUZZERID fuzzerID);
	std::shared_ptr<ILogger> loadLoggerFromModule(TYPE_LOGGERID loggerID, const TYPE_FILESYSTEMPATH& loggerLibPath);
	void unloadLogger(TYPE_LOGGERID loggerID);
	std::shared_ptr<IOutputter> loadOutputterFromModule(TYPE_OUTPUTTERID outputterID, const TYPE_FILESYSTEMPATH& outputterLibPath);
	void unloadOutputter(TYPE_OUTPUTTERID fuzzerID);
};

//====================================================================================================
/**
	\brief Main fuzzing framework class
*/
class CFuzzingManager {
	/**
	\brief CFuzzingManager worker class performing main fuzzing business logic
	*/
	class CWorker {
	public:
		enum EWorkerState { VANILLA, INITIALIZED, PLAYING, STOPPED, ENDED, DEINITIALIZED, INTERRUPTED };

	private:
		CSharedVar<EWorkerState> state;
		CFuzzingManager* pFuzzingManager; // To access links collection

	public:

		CWorker(CFuzzingManager* par_pFuzzingManager);

		void operator()(TYPE_FUZZERID fuzzerID);

		EWorkerState getState();
		void setState(const EWorkerState&);
	};

//----------------------------------------------------------------------------------------------------
	CFuzzerModulesContainer fuzzerModulesContainer;

	std::map<TYPE_FUZZERID, std::shared_ptr<IFuzzer>> fuzzers;
	std::map<TYPE_LOGGERID, std::shared_ptr<ILogger>> loggers;
	std::map<TYPE_OUTPUTTERID, std::shared_ptr<IOutputter>> outputters;
	//std::map<TYPE_FUZZERID, std::list<TYPE_OUTPUTTERID>> links; // {<fuzzerID:{<outputterID#0, outputterID#1,...>}}
	std::map<TYPE_FUZZERID, std::pair<std::unique_ptr<std::mutex>,std::list<std::shared_ptr<IOutputter>>>> links;
	std::map<TYPE_FUZZERID, std::pair<std::unique_ptr<std::mutex>, std::list<std::shared_ptr<ILogger>>>> loggersLinks;
	//links can be accessed by workers and by fuzzing manager itself -> use mutex

	std::map < TYPE_FUZZERID, std::pair < std::shared_ptr<CWorker>, std::thread >> workers;

	std::shared_ptr<spdlog::logger> pLogger;

public:
	using EWorkerState=CFuzzingManager::CWorker::EWorkerState;

	CFuzzingManager();

	//Logging stuff
	void enableLogging(const TYPE_FILESYSTEMPATH& logFilePath);
	void disableLogging();


// Fuzzer-related interface
	/**
	Acquires fuzzer object from fuzzer dynamic lib
	Note it isnt initialized.

	\param[in] fuzzerID Fuzzer identifier
	\param[in] fuzzerLibFilePath Fuzzer dynamic lib path
	*/
	void addFuzzer(TYPE_FUZZERID fuzzerID, const TYPE_FILESYSTEMPATH& fuzzerLibFilePath);
	
	/**
	Removes fuzzer object outa here
	Note it isnt initialized.

	\param[in] fuzzerID Fuzzer identifier
	*/
	void removeFuzzer(TYPE_FUZZERID fuzzerID);
	
	/**
	Initializes fuzzer object with data from config file
	Note it isnt initialized.

	\param[in] fuzzerID Fuzzer identifier
	\param[in] cfgFilePath Fuzzer init config file path 
	*/
	void initFuzzer(TYPE_FUZZERID fuzzerID, const TYPE_FILESYSTEMPATH& cfgFilePath); // Init fuzzer object

	/**
	Deinitializes fuzzer object

	\param[in] fuzzerID Fuzzer identifier
	*/
	void deinitFuzzer(TYPE_FUZZERID fuzzerID);

	/**
	Starts fuzzing process related to selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	*/
	void playFuzzer(TYPE_FUZZERID fuzzerID); // Create thread for fuzzer and run it. This method related to worker, not to fuzzer object
	
	/**
	Stops fuzzing process related to selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	*/
	void stopFuzzer(TYPE_FUZZERID fuzzerID); // Finish fuzzer thread. This method related to worker, not to fuzzer object

// Outputter-related interface
	/**
	Acquires outputter object from fuzzer dynamic lib
	Note it isnt initialized.

	\param[in] outputterID Outputter identifier
	\param[in] outputterLibFilePath Outputter dynamic lib path
	*/
	void addOutputter(TYPE_OUTPUTTERID outputterID, const TYPE_FILESYSTEMPATH& outputterLibFilePath);

	/**
	Removes outputter object outa here
	Note it isnt initialized.

	\param[in] outputterID Outputter identifier
	*/
	void removeOutputter(TYPE_OUTPUTTERID outputterID);

	/**
	Initializes outputter object with data from config file
	Note it isnt initialized.

	\param[in] outputterID Outputter identifier
	\param[in] cfgFilePath Outputter init config file path
	*/
	void initOutputter(TYPE_OUTPUTTERID outputterID, const TYPE_FILESYSTEMPATH& cfgFilePath);

	/**
	Deinitializes outputterID object

	\param[in] outputterID Outputter identifier
	*/
	void deinitOutputter(TYPE_OUTPUTTERID outputterID);

//Logger-related interface
	/**
	Acquires logger object from logger dynamic lib
	Note it isnt initialized.

	\param[in] loggerID Logger identifier
	\param[in] loggerLibFilePath Logger dynamic lib path
	*/
	void addLogger(TYPE_LOGGERID loggerID, const TYPE_FILESYSTEMPATH& loggerLibFilePath);

	/**
	Removes logger object outa here
	Note it isnt initialized.

	\param[in] loggerID Logger identifier
	*/
	void removeLogger(TYPE_LOGGERID loggerID);

	/**
	Initializes logger object with data from config file
	Note it isnt initialized.

	\param[in] loggerID Logger identifier
	\param[in] cfgFilePath Logger init config file path
	*/
	void initLogger(TYPE_LOGGERID loggerID, const TYPE_FILESYSTEMPATH& cfgFilePath);

	/**
	Deinitializes loggerID object

	\param[in] loggerID Logger identifier
	*/
	void deinitLogger(TYPE_LOGGERID loggerID);


// Links-related interface
	/**
	Sets outputters set for selected fuzzer

	\param[in] fuzzerID Outputter identifier
	\param[in] outputters Outputters set
	*/
	void setFuzzerOutputters(TYPE_FUZZERID fuzzerID, const std::list<TYPE_OUTPUTTERID>& par_outputters);

	/**
	Adds outputter to outputters set of selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	\param[in] outputterID Outputter identifier to insert
	*/
	void addFuzzerOutputter(TYPE_FUZZERID fuzzerID, TYPE_OUTPUTTERID outputterID);

	/**
	Removes outputter from outputters set of selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	\param[in] outputterID Outputter to remove
	*/
	void removeFuzzerOutputter(TYPE_FUZZERID fuzzerID, TYPE_OUTPUTTERID outputterID);

// LoggerLinks-related interface
	/**
	Sets loggers set for selected fuzzer

	\param[in] fuzzerID Outputter identifier
	\param[in] loggers Loggers set
	*/
	void setFuzzerLoggers(TYPE_FUZZERID fuzzerID, const std::list<TYPE_LOGGERID>& par_loggers);

	/**
	Adds logger to loggers set of selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	\param[in] loggerID Logger identifier to insert
	*/
	void addFuzzerLogger(TYPE_FUZZERID fuzzerID, TYPE_LOGGERID loggerID);

	/**
	Removes logger from loggers set of selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	\param[in] loggerID Logger to remove
	*/
	void removeFuzzerLogger(TYPE_FUZZERID fuzzerID, TYPE_LOGGERID loggerID);

//State-related interface
	/**
	Provides info about specific fuzzer
	*/
	std::tuple<CFuzzingManager::EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID>> getFuzzerState(TYPE_FUZZERID fuzzerID);

	/**
	Provides overall info about current fuzzer framework state
	*/
	std::vector<std::tuple<TYPE_FUZZERID, EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID> >> getState();

//----------------------------------------------------------------------------------------------------
	class ExFuzzingManager : public ExException {
	public:
		ExFuzzingManager(const std::string& par_msg);
	};
};
#endif