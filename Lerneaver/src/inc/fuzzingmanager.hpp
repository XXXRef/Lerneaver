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
#include "config.hpp"
#include "i_fuzzer.hpp"
#include "i_logger.hpp"
#include "i_outputter.hpp"

#include "fuzzermodulesmanager.hpp"

#include "spdlog/spdlog.h"

//====================================================================================================
/**
	\brief Main fuzzing framework class
*/
class CFuzzingManager {
public:
	using TYPE_FUZZERID=config::TYPE_FUZZERID;
	using TYPE_LOGGERID=config::TYPE_LOGGERID;
	using TYPE_OUTPUTTERID=config::TYPE_OUTPUTTERID;
	using TYPE_MODID=config::TYPE_MODID;
	using TYPE_CONFIGID=config::TYPE_CONFIGID;

private:
	/**
	\brief CFuzzingManager worker class performing main fuzzing business logic
	*/
	class CWorker {
	public:
		enum EWorkerState { VANILLA, INITIALIZED, PLAYING, STOPPED, ENDED, DEINITIALIZED, INTERRUPTED };

	private:
		CSharedVar<EWorkerState> state;
		CFuzzingManager *pFuzzingManager; // To access links collection

	public:

		CWorker(CFuzzingManager *par_pFuzzingManager);

		void operator()(const TYPE_FUZZERID &fuzzerID);

		EWorkerState getState();
		void setState(const EWorkerState &);
	};

//----------------------------------------------------------------------------------------------------
	CFuzzerModulesManager fuzzerModulesContainer;

	std::map<TYPE_FUZZERID, std::shared_ptr<IFuzzer>> fuzzers;
	std::map<TYPE_LOGGERID, std::shared_ptr<ILogger>> loggers;
	std::map<TYPE_OUTPUTTERID, std::shared_ptr<IOutputter>> outputters;
	//std::map<TYPE_FUZZERID, std::list<TYPE_OUTPUTTERID>> links; // {<fuzzerID:{<outputterID#0, outputterID#1,...>}}
	std::map<TYPE_FUZZERID, std::pair<std::unique_ptr<std::mutex>,std::list<std::shared_ptr<IOutputter>>>> links;
	std::map<TYPE_FUZZERID, std::pair<std::unique_ptr<std::mutex>, std::list<std::shared_ptr<ILogger>>>> loggersLinks;
	//links can be accessed by workers and by fuzzing manager itself -> use mutex

	std::map <TYPE_FUZZERID, std::pair< std::shared_ptr<CWorker>, std::thread>> workers;

	std::shared_ptr<spdlog::logger> pLogger;
	
	using TYPE_MANAGERID=unsigned;
	TYPE_MANAGERID fuzzingManagerID;
	
public:
	using EWorkerState=CFuzzingManager::CWorker::EWorkerState;

	CFuzzingManager();

	//Logging stuff
	void enableLogging(const config::platform::TYPE_FILESYSTEMPATH &logFilePath);
	void disableLogging();


// Fuzzer-related interface
	/**
	Acquires fuzzer object from fuzzer dynamic lib
	Note it isnt initialized.

	\param[in] fuzzerID Fuzzer identifier
	\param[in] fuzzerLibFilePath Fuzzer module ID
	*/
	void addFuzzer(const TYPE_FUZZERID &fuzzerID, const TYPE_MODID &fuzzerModID);
	
	/**
	Removes fuzzer object outa here
	Note it isnt initialized.

	\param[in] fuzzerID Fuzzer identifier
	*/
	void removeFuzzer(const TYPE_FUZZERID &fuzzerID);
	
	/**
	Initializes fuzzer object with data from config file
	Note it isnt initialized.

	\param[in] fuzzerID Fuzzer identifier
	\param[in] cfgFilePath Fuzzer init config file ID 
	*/
	void initFuzzer(const TYPE_FUZZERID &fuzzerID, const TYPE_CONFIGID &cfgFileID); // Init fuzzer object

	/**
	Deinitializes fuzzer object

	\param[in] fuzzerID Fuzzer identifier
	*/
	void deinitFuzzer(const TYPE_FUZZERID &fuzzerID);

	/**
	Starts fuzzing process related to selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	*/
	void playFuzzer(const TYPE_FUZZERID &fuzzerID); // Create thread for fuzzer and run it. This method related to worker, not to fuzzer object
	
	/**
	Stops fuzzing process related to selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	*/
	void stopFuzzer(const TYPE_FUZZERID &fuzzerID); // Finish fuzzer thread. This method related to worker, not to fuzzer object

// Outputter-related interface
	/**
	Acquires outputter object from fuzzer dynamic lib
	Note it isnt initialized.

	\param[in] outputterID Outputter identifier
	\param[in] outputterLibFilePath Outputter module ID
	*/
	void addOutputter(const TYPE_OUTPUTTERID &outputterID, const TYPE_MODID &outputterModID);

	/**
	Removes outputter object outa here
	Note it isnt initialized.

	\param[in] outputterID Outputter identifier
	*/
	void removeOutputter(const TYPE_OUTPUTTERID &outputterID);

	/**
	Initializes outputter object with data from config file
	Note it isnt initialized.

	\param[in] outputterID Outputter identifier
	\param[in] cfgFilePath Outputter init config ID
	*/
	void initOutputter(const TYPE_OUTPUTTERID &outputterID, const TYPE_CONFIGID &cfgFileID);

	/**
	Deinitializes outputterID object

	\param[in] outputterID Outputter identifier
	*/
	void deinitOutputter(const TYPE_OUTPUTTERID &outputterID);

//Logger-related interface
	/**
	Acquires logger object from logger dynamic lib
	Note it isnt initialized.

	\param[in] loggerID Logger identifier
	\param[in] loggerLibFilePath Logger module ID
	*/
	void addLogger(const TYPE_LOGGERID &loggerID, const TYPE_MODID &loggerLibFilePath);

	/**
	Removes logger object outta here
	Note it isnt initialized.

	\param[in] loggerID Logger identifier
	*/
	void removeLogger(const TYPE_LOGGERID &loggerID);

	/**
	Initializes logger object with data from config file
	Note it isnt initialized.

	\param[in] loggerID Logger identifier
	\param[in] cfgFilePath Logger init config file path
	*/
	void initLogger(const TYPE_LOGGERID &loggerID, const TYPE_CONFIGID &cfgFilePath);

	/**
	Deinitializes loggerID object

	\param[in] loggerID Logger identifier
	*/
	void deinitLogger(const TYPE_LOGGERID &loggerID);


// Links-related interface
	/**
	Sets outputters set for selected fuzzer

	\param[in] fuzzerID Outputter identifier
	\param[in] outputters Outputters set
	*/
	void setFuzzerOutputters(const TYPE_FUZZERID &fuzzerID, const std::list<TYPE_OUTPUTTERID> &par_outputters);

	/**
	Adds outputter to outputters set of selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	\param[in] outputterID Outputter identifier to insert
	*/
	void addFuzzerOutputter(const TYPE_FUZZERID &fuzzerID, const TYPE_OUTPUTTERID &outputterID);

	/**
	Removes outputter from outputters set of selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	\param[in] outputterID Outputter to remove
	*/
	void removeFuzzerOutputter(const TYPE_FUZZERID &fuzzerID, const TYPE_OUTPUTTERID &outputterID);

// LoggerLinks-related interface
	/**
	Sets loggers set for selected fuzzer

	\param[in] fuzzerID Outputter identifier
	\param[in] loggers Loggers set
	*/
	void setFuzzerLoggers(const TYPE_FUZZERID &fuzzerID, const std::list<TYPE_LOGGERID> &par_loggers);

	/**
	Adds logger to loggers set of selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	\param[in] loggerID Logger identifier to insert
	*/
	void addFuzzerLogger(const TYPE_FUZZERID &fuzzerID, const TYPE_LOGGERID &loggerID);

	/**
	Removes logger from loggers set of selected fuzzer

	\param[in] fuzzerID Fuzzer identifier
	\param[in] loggerID Logger to remove
	*/
	void removeFuzzerLogger(const TYPE_FUZZERID &fuzzerID, const TYPE_LOGGERID &loggerID);

//State-related interface
	/**
	Provides info about specific fuzzer
	*/
	std::tuple<CFuzzingManager::EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID>> getFuzzerState(const TYPE_FUZZERID &fuzzerID);

	/**
	Provides overall info about current fuzzer framework state
	*/
	std::vector<std::tuple<TYPE_FUZZERID, EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID> >> getState();

//Exceptions
	class ExFuzzingManager : public ExEx {
		std::string exInfo;
	public:
		ExFuzzingManager(const std::string &par_exInfo) : exInfo(par_exInfo) {}
		std::string getInfo() const override {
			return std::string("[ExFuzzingManager EXCEPTION] ") + exInfo;
		}
	};
};
#endif