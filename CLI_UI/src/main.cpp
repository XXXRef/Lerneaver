/**
\file

CLI_UI main module
*/

#include <iostream>
#include <cstddef>
#include <string>
#include <sstream>

#include "fuzzingmanager.hpp"

#include "types.hpp"

#if defined PLATFORM_WINDOWS
	using MAINCFG_STRTYPE=std::string;
	#define MAINCFG_STD_IN_OBJ std::cin
	#define MAINCFG_STD_OUT_OBJ std::cout
	using STRSTREAM=std::stringstream;
	//Commands
	#define MAINCFG_CMD_EXIT "EXIT"
	#define MAINCFG_CMD_EXIT_LC "exit"
	#define MAINCFG_CMD_LS "LS"
	#define MAINCFG_CMD_LS_LC "ls"
	#define MAINCFG_CMD_STATE "STATE"
	#define MAINCFG_CMD_STATE_LC "state"
	#define MAINCFG_CMD_ADD "ADD"
	#define MAINCFG_CMD_ADD_LC "add"
	#define MAINCFG_CMD_RM "RM"
	#define MAINCFG_CMD_RM_LC "rm"
	#define MAINCFG_CMD_INIT "INIT"
	#define MAINCFG_CMD_INIT_LC "init"
	#define MAINCFG_CMD_DEINIT "DEINIT"
	#define MAINCFG_CMD_DEINIT_LC "deinit"
	#define MAINCFG_CMD_PLAY "PLAY"
	#define MAINCFG_CMD_PLAY_LC "play"
	#define MAINCFG_CMD_STOP "STOP"
	#define MAINCFG_CMD_STOP_LC "stop"
	#define MAINCFG_CMD_ADDLOGGER "ADD_LOGGER"
	#define MAINCFG_CMD_ADDLOGGER_LC "add_logger"
	#define MAINCFG_CMD_RMLOGGER "RM_LOGGER"
	#define MAINCFG_CMD_RMLOGGER_LC "rm_logger"
	#define MAINCFG_CMD_INITLOGGER "INIT_LOGGER"
	#define MAINCFG_CMD_INITLOGGER_LC "init_logger"
	#define MAINCFG_CMD_DEINITLOGGER "DEINIT_LOGGER"
	#define MAINCFG_CMD_DEINITLOGGER_LC "deinit_logger"
	#define MAINCFG_CMD_LINKLOGGER "LINK_LOGGER"
	#define MAINCFG_CMD_LINKLOGGER_LC "link_logger"
	#define MAINCFG_CMD_UNLINKLOGGER "UNLINK_LOGGER"
	#define MAINCFG_CMD_UNLINKLOGGER_LC "unlink_logger"
	#define MAINCFG_CMD_ADDOUTPUTTER "ADD_OUTPUTTER"
	#define MAINCFG_CMD_ADDOUTPUTTER_LC "add_outputter"
	#define MAINCFG_CMD_RMOUTPUTTER "RM_OUTPUTTER"
	#define MAINCFG_CMD_RMOUTPUTTER_LC "rm_outputter"
	#define MAINCFG_CMD_INITOUTPUTTER "INIT_OUTPUTTER"
	#define MAINCFG_CMD_INITOUTPUTTER_LC "init_outputter"
	#define MAINCFG_CMD_DEINITOUTPUTTER "DEINIT_OUTPUTTER"
	#define MAINCFG_CMD_DEINITOUTPUTTER_LC "deinit_outputter"
	#define MAINCFG_CMD_LINKOUTPUTTER "LINK_OUTPUTTER"
	#define MAINCFG_CMD_LINKOUTPUTTER_LC "link_outputter"
	#define MAINCFG_CMD_UNLINKOUTPUTTER "UNLINK_OUTPUTTER"
	#define MAINCFG_CMD_UNLINKOUTPUTTER_LC "unlink_outputter"

	#define MAINCFG_LITERAL_EMPTY ""
	#define MAINCFG_LITERAL_SPACE " "

#elif defined PLATFORM_NIX
	using MAINCFG_STRTYPE=std::string;
	#define MAINCFG_STD_IN_OBJ std::cin
	#define MAINCFG_STD_OUT_OBJ std::cout
	using STRSTREAM=std::stringstream;
	//Commands
	#define MAINCFG_CMD_EXIT "EXIT"
	#define MAINCFG_CMD_EXIT_LC "exit"
	#define MAINCFG_CMD_LS "LS"
	#define MAINCFG_CMD_LS_LC "ls"
	#define MAINCFG_CMD_STATE "STATE"
	#define MAINCFG_CMD_STATE_LC "state"
	#define MAINCFG_CMD_ADD "ADD"
	#define MAINCFG_CMD_ADD_LC "add"
	#define MAINCFG_CMD_RM "RM"
	#define MAINCFG_CMD_RM_LC "rm"
	#define MAINCFG_CMD_INIT "INIT"
	#define MAINCFG_CMD_INIT_LC "init"
	#define MAINCFG_CMD_DEINIT "DEINIT"
	#define MAINCFG_CMD_DEINIT_LC "deinit"
	#define MAINCFG_CMD_PLAY "PLAY"
	#define MAINCFG_CMD_PLAY_LC "play"
	#define MAINCFG_CMD_STOP "STOP"
	#define MAINCFG_CMD_STOP_LC "stop"
	#define MAINCFG_CMD_ADDLOGGER "ADD_LOGGER"
	#define MAINCFG_CMD_ADDLOGGER_LC "add_logger"
	#define MAINCFG_CMD_RMLOGGER "RM_LOGGER"
	#define MAINCFG_CMD_RMLOGGER_LC "rm_logger"
	#define MAINCFG_CMD_INITLOGGER "INIT_LOGGER"
	#define MAINCFG_CMD_INITLOGGER_LC "init_logger"
	#define MAINCFG_CMD_DEINITLOGGER "DEINIT_LOGGER"
	#define MAINCFG_CMD_DEINITLOGGER_LC "deinit_logger"
	#define MAINCFG_CMD_LINKLOGGER "LINK_LOGGER"
	#define MAINCFG_CMD_LINKLOGGER_LC "link_logger"
	#define MAINCFG_CMD_UNLINKLOGGER "UNLINK_LOGGER"
	#define MAINCFG_CMD_UNLINKLOGGER_LC "unlink_logger"
	#define MAINCFG_CMD_ADDOUTPUTTER "ADD_OUTPUTTER"
	#define MAINCFG_CMD_ADDOUTPUTTER_LC "add_outputter"
	#define MAINCFG_CMD_RMOUTPUTTER "RM_OUTPUTTER"
	#define MAINCFG_CMD_RMOUTPUTTER_LC "rm_outputter"
	#define MAINCFG_CMD_INITOUTPUTTER "INIT_OUTPUTTER"
	#define MAINCFG_CMD_INITOUTPUTTER_LC "init_outputter"
	#define MAINCFG_CMD_DEINITOUTPUTTER "DEINIT_OUTPUTTER"
	#define MAINCFG_CMD_DEINITOUTPUTTER_LC "deinit_outputter"
	#define MAINCFG_CMD_LINKOUTPUTTER "LINK_OUTPUTTER"
	#define MAINCFG_CMD_LINKOUTPUTTER_LC "link_outputter"
	#define MAINCFG_CMD_UNLINKOUTPUTTER "UNLINK_OUTPUTTER"
	#define MAINCFG_CMD_UNLINKOUTPUTTER_LC "unlink_outputter"

	#define MAINCFG_LITERAL_EMPTY ""
	#define MAINCFG_LITERAL_SPACE " "

	#define MAINCFG_LITERAL_EMPTY ""
	#define MAINCFG_LITERAL_SPACE " "

#endif

std::map<CFuzzingManager::EWorkerState, std::string> statesNames = {
	{ CFuzzingManager::EWorkerState::VANILLA, "VANILLA"},
	{ CFuzzingManager::EWorkerState::INITIALIZED, "INITIALIZED" },
	{ CFuzzingManager::EWorkerState::PLAYING, "PLAYING" },
	{ CFuzzingManager::EWorkerState::STOPPED, "STOPPED" },
	{ CFuzzingManager::EWorkerState::ENDED, "ENDED" },
	{ CFuzzingManager::EWorkerState::DEINITIALIZED, "DEINITIALIZED" },
	{ CFuzzingManager::EWorkerState::INTERRUPTED, "INTERRUPTED" }
};

//Usage: FuzzingFramework cfg_filePath. cfg_filePath implied to be wide string
#if defined PLATFORM_WINDOWS
	int wmain(int argc, wchar_t** argv) {
#elif defined PLATFORM_UNIX
	int main(int argc, char** argv) {
#endif

	CFuzzingManager fuzzingManager;

	// Handle user input
	MAINCFG_STRTYPE userInputString;
	while (true) {
		std::cout << "> ";//TIP we can mix std::cout and std::wcout usage
		std::getline(MAINCFG_STD_IN_OBJ, userInputString);
		STRSTREAM strStream(userInputString);
		MAINCFG_STRTYPE firstWord;
		strStream >> firstWord;
		if ((MAINCFG_CMD_EXIT == firstWord) || (MAINCFG_CMD_EXIT_LC == firstWord)) {
			// Format: "EXIT"
			break;
		}

		if ((MAINCFG_CMD_LS == firstWord) || (MAINCFG_CMD_LS_LC == firstWord)) {
			// Format: "LS"
			decltype(fuzzingManager.getState()) state;
			try {
				state=fuzzingManager.getState();
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}

			for (const auto &fuzzerState: state) {
				// {1:fuzzerWorkerState, 2:fuzzerLoggers, 3:fuzzerOutputters}
				std::cout << L"fuzzerID="; MAINCFG_STD_OUT_OBJ << std::get<0>(fuzzerState) << std::endl;
				
				std::cout << "\t" << "state=" << ::statesNames[std::get<1>(fuzzerState)] << std::endl;
				
				std::cout << "\t" << "loggers={";
				auto loggersIDsList = std::get<2>(fuzzerState);
				for (const auto &e: loggersIDsList) {
					std::cout << " "; MAINCFG_STD_OUT_OBJ << e;
				}
				std::cout << " }" << std::endl;

				std::cout << "\t" << "outputters={";
				auto outputtersIDsList = std::get<3>(fuzzerState);
				for (const auto &e: outputtersIDsList) {
					std::cout << " "; MAINCFG_STD_OUT_OBJ << e;
				}
				std::cout << " }" << std::endl;

				std::cout << std::endl;
			}
		}

	//Fuzzer-related commands
		else if ((MAINCFG_CMD_ADD == firstWord) || (MAINCFG_CMD_ADD_LC == firstWord)) {
			//Format: ADD_FUZZER fuzzerID fuzzerLibPath
			MAINCFG_STRTYPE fuzzerIDStr;
			MAINCFG_STRTYPE fuzzerLibPathStr;
			strStream >> fuzzerIDStr;

			// Dealing with spaces in job lib path
			MAINCFG_STRTYPE fuzzerLibPathPartStr;
			while (strStream.eof() != true) {
				if (fuzzerLibPathPartStr != MAINCFG_LITERAL_EMPTY) {
					fuzzerLibPathStr += MAINCFG_LITERAL_SPACE;
				}
				strStream >> fuzzerLibPathPartStr;
				fuzzerLibPathStr += fuzzerLibPathPartStr;
			}
			try {
				fuzzingManager.addFuzzer(fuzzerIDStr.c_str(), fuzzerLibPathStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_RM == firstWord) || (MAINCFG_CMD_RM_LC == firstWord)) {
			//Format: RM_FUZZER fuzzerID
			MAINCFG_STRTYPE fuzzerIDStr;
			strStream >> fuzzerIDStr;
			try {
				fuzzingManager.removeFuzzer(fuzzerIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_INIT == firstWord) || (MAINCFG_CMD_INIT_LC == firstWord)) {
			//Format: INIT_FUZZER fuzzerID cfgFilePath
			MAINCFG_STRTYPE fuzzerIDStr;
			MAINCFG_STRTYPE cfgFilePathStr;
			strStream >> fuzzerIDStr;

			// Dealing with spaces in job lib path
			MAINCFG_STRTYPE cfgFilePathPartStr;
			while (strStream.eof() != true) {
				if (cfgFilePathPartStr != MAINCFG_LITERAL_EMPTY) {
					cfgFilePathStr += MAINCFG_LITERAL_SPACE;
				}
				strStream >> cfgFilePathPartStr;
				cfgFilePathStr += cfgFilePathPartStr;
			}
			try {
				fuzzingManager.initFuzzer(fuzzerIDStr.c_str(), cfgFilePathStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_DEINIT == firstWord) || (MAINCFG_CMD_DEINIT_LC == firstWord)) {
			//Format: DEINIT_FUZZER fuzzerID
			MAINCFG_STRTYPE fuzzerIDStr;
			strStream >> fuzzerIDStr;
			try {
				fuzzingManager.deinitFuzzer(fuzzerIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_PLAY == firstWord) || (MAINCFG_CMD_PLAY_LC == firstWord)) {
			//Format: PLAY fuzzerID
			MAINCFG_STRTYPE fuzzerIDStr;
			strStream >> fuzzerIDStr;
			try {
				fuzzingManager.playFuzzer(fuzzerIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_STOP == firstWord) || (MAINCFG_CMD_STOP_LC == firstWord)) {
			//Format: STOP fuzzerID
			MAINCFG_STRTYPE fuzzerIDStr;
			strStream >> fuzzerIDStr;
			try {
				fuzzingManager.stopFuzzer(fuzzerIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}

	// Logger-related commands
		else if ((MAINCFG_CMD_ADDLOGGER == firstWord) || (MAINCFG_CMD_ADDLOGGER_LC == firstWord)) {
			//Format: ADD_LOGGER loggerID loggerLibPath
			MAINCFG_STRTYPE loggerIDStr;
			MAINCFG_STRTYPE loggerLibPathStr;
			strStream >> loggerIDStr;

			// Dealing with spaces in job lib path
			MAINCFG_STRTYPE loggerLibPathPartStr;
			while (strStream.eof() != true) {
				if (loggerLibPathPartStr != MAINCFG_LITERAL_EMPTY) {
					loggerLibPathStr += MAINCFG_LITERAL_SPACE;
				}
				strStream >> loggerLibPathPartStr;
				loggerLibPathStr += loggerLibPathPartStr;
			}
			try {
				fuzzingManager.addLogger(loggerIDStr.c_str(), loggerLibPathPartStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_RMLOGGER == firstWord) || (MAINCFG_CMD_RMLOGGER_LC == firstWord)) {
			//Format: RM_LOGGER loggerID
			MAINCFG_STRTYPE loggerIDStr;
			strStream >> loggerIDStr;
			try {
				fuzzingManager.removeLogger(loggerIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_INITLOGGER == firstWord) || (MAINCFG_CMD_INITLOGGER_LC == firstWord)) {
			//Format: INIT_LOGGER loggerID cfgFilePath
			MAINCFG_STRTYPE loggerIDStr;
			MAINCFG_STRTYPE cfgFilePathStr;
			strStream >> loggerIDStr;

			// Dealing with spaces in job lib path
			MAINCFG_STRTYPE cfgFilePathPartStr;
			while (strStream.eof() != true) {
				if (cfgFilePathPartStr != MAINCFG_LITERAL_EMPTY) {
					cfgFilePathStr += MAINCFG_LITERAL_SPACE;
				}
				strStream >> cfgFilePathPartStr;
				cfgFilePathStr += cfgFilePathPartStr;
			}
			try {
				fuzzingManager.initLogger(loggerIDStr.c_str(), cfgFilePathStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_DEINITLOGGER == firstWord) || (MAINCFG_CMD_DEINITLOGGER_LC == firstWord)) {
			//Format: DEINIT_LOGGER loggerID
			MAINCFG_STRTYPE loggerIDStr;
			strStream >> loggerIDStr;
			try {
				fuzzingManager.deinitLogger(loggerIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_LINKLOGGER == firstWord) || (MAINCFG_CMD_LINKLOGGER_LC == firstWord)) {
			//Format: LINK fuzzerID loggerID
			MAINCFG_STRTYPE fuzzerIDStr;
			MAINCFG_STRTYPE loggerIDStr;
			strStream >> fuzzerIDStr;
			strStream >> loggerIDStr;
			try {
				fuzzingManager.addFuzzerLogger(fuzzerIDStr.c_str(), loggerIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_UNLINKLOGGER == firstWord) || (MAINCFG_CMD_UNLINKLOGGER_LC == firstWord)) {
			//Format: UNLINK fuzzerID loggerID
			MAINCFG_STRTYPE fuzzerIDStr;
			MAINCFG_STRTYPE loggerIDStr;
			strStream >> fuzzerIDStr;
			strStream >> loggerIDStr;
			try {
				fuzzingManager.removeFuzzerLogger(fuzzerIDStr.c_str(), loggerIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
	// Outputter-related commands
		else if ((MAINCFG_CMD_ADDOUTPUTTER == firstWord) || (MAINCFG_CMD_ADDOUTPUTTER_LC == firstWord)) {
			//Format: ADD_OUTPUTTER outputterID outputterLibPath
			MAINCFG_STRTYPE outputterIDStr;
			MAINCFG_STRTYPE outputterLibPathStr;
			strStream >> outputterIDStr;

			// Dealing with spaces in job lib path
			MAINCFG_STRTYPE outputterLibPathPartStr;
			while (strStream.eof() != true) {
				if (outputterLibPathPartStr != MAINCFG_LITERAL_EMPTY) {
					outputterLibPathStr += MAINCFG_LITERAL_SPACE;
				}
				strStream >> outputterLibPathPartStr;
				outputterLibPathStr += outputterLibPathPartStr;
			}
			try {
				fuzzingManager.addOutputter(outputterIDStr.c_str(), outputterLibPathStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_RMOUTPUTTER == firstWord) || (MAINCFG_CMD_RMOUTPUTTER_LC == firstWord)) {
			//Format: RM_OUTPUTTER outputterID
			MAINCFG_STRTYPE outputterIDStr;
			strStream >> outputterIDStr;
			try {
				fuzzingManager.removeOutputter(outputterIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_INITOUTPUTTER == firstWord) || (MAINCFG_CMD_INITOUTPUTTER_LC == firstWord)) {
			//Format: INIT_OUTPUTTER outputterID cfgFilePath
			MAINCFG_STRTYPE outputterIDStr;
			MAINCFG_STRTYPE cfgFilePathStr;
			strStream >> outputterIDStr;

			// Dealing with spaces in job lib path
			MAINCFG_STRTYPE cfgFilePathPartStr;
			while (strStream.eof() != true) {
				if (cfgFilePathPartStr != MAINCFG_LITERAL_EMPTY) {
					cfgFilePathStr += MAINCFG_LITERAL_SPACE;
				}
				strStream >> cfgFilePathPartStr;
				cfgFilePathStr += cfgFilePathPartStr;
			}
			try {
				fuzzingManager.initOutputter(outputterIDStr.c_str(), cfgFilePathStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_DEINITOUTPUTTER == firstWord) || (MAINCFG_CMD_DEINITOUTPUTTER_LC == firstWord)) {
			//Format: DEINIT_OUTPUTTER outputterID
			MAINCFG_STRTYPE outputterIDStr;
			strStream >> outputterIDStr;
			try {
				fuzzingManager.deinitOutputter(outputterIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}

	// Links-related commands
		else if ((MAINCFG_CMD_LINKOUTPUTTER == firstWord) || (MAINCFG_CMD_LINKOUTPUTTER_LC == firstWord)) {
			//Format: LINK fuzzerID outputterID
			MAINCFG_STRTYPE fuzzerIDStr;
			MAINCFG_STRTYPE outputterIDStr;
			strStream >> fuzzerIDStr;
			strStream >> outputterIDStr;
			try {
				fuzzingManager.addFuzzerOutputter(fuzzerIDStr.c_str(), outputterIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		else if ((MAINCFG_CMD_UNLINKOUTPUTTER == firstWord) || (MAINCFG_CMD_UNLINKOUTPUTTER_LC == firstWord)) {
			//Format: UNLINK fuzzerID outputterID
			MAINCFG_STRTYPE fuzzerIDStr;
			MAINCFG_STRTYPE outputterIDStr;
			strStream >> fuzzerIDStr;
			strStream >> outputterIDStr;
			try {
				fuzzingManager.removeFuzzerOutputter(fuzzerIDStr.c_str(), outputterIDStr.c_str());
			}
			catch (CFuzzingManager::ExFuzzingManager &e) {
				std::cout << "[ERROR] " << e.getInfo() << std::endl;
				continue;
			}
		}
		
		else {
			std::cout << "\t" << "[ERROR] Unknown command" << std::endl;
		}
	}

	return EXIT_SUCCESS;
}