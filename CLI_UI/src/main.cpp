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
	#define MAINCFG_CMD_ADDOUTPUTTER "ADD_OUTPUTTER"
	#define MAINCFG_CMD_ADDOUTPUTTER_LC "add_outputter"
	#define MAINCFG_CMD_RMOUTPUTTER "RM_OUTPUTTER"
	#define MAINCFG_CMD_RMOUTPUTTER_LC "rm_outputter"
	#define MAINCFG_CMD_INITOUTPUTTER "INIT_OUTPUTTER"
	#define MAINCFG_CMD_INITOUTPUTTER_LC "init_outputter"
	#define MAINCFG_CMD_DEINITOUTPUTTER "DEINIT_OUTPUTTER"
	#define MAINCFG_CMD_DEINITOUTPUTTER_LC "deinit_outputter"
	#define MAINCFG_CMD_LINKLOGGER "LINK_LOGGER"
	#define MAINCFG_CMD_LINKLOGGER_LC "link_logger"
	#define MAINCFG_CMD_LINKOUTPUTTER "LINK_OUTPUTTER"
	#define MAINCFG_CMD_LINKOUTPUTTER_LC "link_outputter"

#elif defined PLATFORM_NIX
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
	#define MAINCFG_CMD_ADDOUTPUTTER "ADD_OUTPUTTER"
	#define MAINCFG_CMD_ADDOUTPUTTER_LC "add_outputter"
	#define MAINCFG_CMD_RMOUTPUTTER "RM_OUTPUTTER"
	#define MAINCFG_CMD_RMOUTPUTTER_LC "rm_outputter"
	#define MAINCFG_CMD_INITOUTPUTTER "INIT_OUTPUTTER"
	#define MAINCFG_CMD_INITOUTPUTTER_LC "init_outputter"
	#define MAINCFG_CMD_DEINITOUTPUTTER "DEINIT_OUTPUTTER"
	#define MAINCFG_CMD_DEINITOUTPUTTER_LC "deinit_outputter"
	#define MAINCFG_CMD_LINKLOGGER "LINK_LOGGER"
	#define MAINCFG_CMD_LINKLOGGER_LC "link_logger"
	#define MAINCFG_CMD_LINKOUTPUTTER "LINK_OUTPUTTER"
	#define MAINCFG_CMD_LINKOUTPUTTER_LC "link_outputter"
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
		if ((firstWord == MAINCFG_CMD_EXIT) || (firstWord == MAINCFG_CMD_EXIT_LC)) {
			// Format: "EXIT"
			break;
		}

		if ((firstWord == MAINCFG_CMD_LS) || (firstWord == MAINCFG_CMD_LS_LC)) {
			// Format: "LS"
			std::vector<std::tuple<TYPE_FUZZERID, CFuzzingManager::EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID> >> state;
			try {
				state=fuzzingManager.getState();
			}
			catch (CFuzzingManager::ExFuzzinManager &e) {
				std::cout << "[ERROR] " << e.getinfo() << std::endl;
				continue;

			}

			for (const auto& fuzzerState: state) {
				// {1:fuzzerWorkerState, 2:fuzzerLoggers, 3:fuzzerOutputters}
				std::wcout << L"fuzzerID=" << std::get<0>(fuzzerState) << std::endl;
				
				std::wcout << L"\t" << L"state=" << ::statesNames[std::get<1>(fuzzerState)] << std::endl;
				
				std::wcout << L"\t" << L"loggers={";
				auto loggersIDsList = std::get<2>(fuzzerState);
				for (const auto& e: loggersIDsList) {
					std::wcout << L" " << e;
				}
				std::wcout << " }";
				std::wcout << std::endl;

				std::wcout << L"\t" << L"outputters={";
				auto outputtersIDsList = std::get<3>(fuzzerState);
				for (const auto& e : outputtersIDsList) {
					std::wcout << L" " << e;
				}
				std::wcout << " }";

				std::wcout << std::endl;

				std::wcout << std::endl;
			}
		}

	//Fuzzer-related commands
		else if ((firstWord == MAINCFG_CMD_ADD) || (firstWord == MAINCFG_CMD_ADD_LC)) {
			//Format: ADD_FUZZER fuzzerID fuzzerLibPath
			std::wstring fuzzerIDStr;
			std::wstring fuzzerLibPathStr;
			strStream >> fuzzerIDStr;

			// Dealing with spaces in job lib path
			std::wstring fuzzerLibPathPartStr;
			while (strStream.eof() != true) {
				if (fuzzerLibPathPartStr != L"") {
					fuzzerLibPathStr += L" ";
				}
				strStream >> fuzzerLibPathPartStr;
				fuzzerLibPathStr += fuzzerLibPathPartStr;
			}
			try {
				fuzzingManager.addFuzzer(fuzzerIDStr, fuzzerLibPathStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_CMD_RM) || (firstWord == MAINCFG_CMD_RM_LC)) {
			//Format: RM_FUZZER fuzzerID
			std::wstring fuzzerIDStr;
			strStream >> fuzzerIDStr;
			try {
				fuzzingManager.removeFuzzer(fuzzerIDStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_CMD_INIT) || (firstWord == MAINCFG_CMD_INIT_LC)) {
			//Format: INIT_FUZZER fuzzerID cfgFilePath
			std::wstring fuzzerIDStr;
			std::wstring cfgFilePathStr;
			strStream >> fuzzerIDStr;

			// Dealing with spaces in job lib path
			std::wstring cfgFilePathPartStr;
			while (strStream.eof() != true) {
				if (cfgFilePathPartStr != L"") {
					cfgFilePathStr += L" ";
				}
				strStream >> cfgFilePathPartStr;
				cfgFilePathStr += cfgFilePathPartStr;
			}
			try {
				fuzzingManager.initFuzzer(fuzzerIDStr, cfgFilePathStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_CMD_DEINIT) || (firstWord == MAINCFG_CMD_DEINIT_LC)) {
			//Format: DEINIT_FUZZER fuzzerID
			std::wstring fuzzerIDStr;
			strStream >> fuzzerIDStr;
			try {
				fuzzingManager.deinitFuzzer(fuzzerIDStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_CMD_PLAY) || (firstWord == MAINCFG_CMD_PLAY_LC)) {
			//Format: PLAY fuzzerID
			std::wstring fuzzerIDStr;
			strStream >> fuzzerIDStr;
			try {
				fuzzingManager.playFuzzer(fuzzerIDStr);
			}
			catch (...) {
				std::wcout << "[ERROR] Failed playing fuzzer!" << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_CMD_STOP) || (firstWord == MAINCFG_CMD_STOP_LC)) {
			//Format: STOP fuzzerID
			std::wstring fuzzerIDStr;
			strStream >> fuzzerIDStr;
			try {
				fuzzingManager.stopFuzzer(fuzzerIDStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}

	// Logger-related commands
		else if ((firstWord == MAINCFG_ADDLOGGER) || (firstWord == MAINCFG_ADDLOGGER_LC)) {
			//Format: ADD_LOGGER loggerID loggerLibPath
			std::wstring loggerIDStr;
			std::wstring loggerLibPathStr;
			strStream >> loggerIDStr;

			// Dealing with spaces in job lib path
			std::wstring loggerLibPathPartStr;
			while (strStream.eof() != true) {
				if (loggerLibPathPartStr != L"") {
					loggerLibPathStr += L" ";
				}
				strStream >> loggerLibPathPartStr;
				loggerLibPathStr += loggerLibPathPartStr;
			}
			try {
				fuzzingManager.addLogger(loggerIDStr, loggerLibPathPartStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_RMLOGGER) || (firstWord == MAINCFG_RMLOGGER_LC)) {
			//Format: RM_LOGGER loggerID
			std::wstring loggerIDStr;
			strStream >> loggerIDStr;
			try {
				fuzzingManager.removeLogger(loggerIDStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_INITLOGGER) || (firstWord == MAINCFG_INITLOGGER_LC)) {
			//Format: INIT_LOGGER loggerID cfgFilePath
			std::wstring loggerIDStr;
			std::wstring cfgFilePathStr;
			strStream >> loggerIDStr;

			// Dealing with spaces in job lib path
			std::wstring cfgFilePathPartStr;
			while (strStream.eof() != true) {
				if (cfgFilePathPartStr != L"") {
					cfgFilePathStr += L" ";
				}
				strStream >> cfgFilePathPartStr;
				cfgFilePathStr += cfgFilePathPartStr;
			}
			try {
				fuzzingManager.initLogger(loggerIDStr, cfgFilePathStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_DEINITLOGGER) || (firstWord == MAINCFG_DEINITLOGGER_LC)) {
			//Format: DEINIT_LOGGER loggerID
			std::wstring loggerIDStr;
			strStream >> loggerIDStr;
			try {
				fuzzingManager.deinitLogger(loggerIDStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}

	// Outputter-related commands
		else if ((firstWord == MAINCFG_ADDOUTPUTTER) || (firstWord == MAINCFG_ADDOUTPUTTER_LC)) {
			//Format: ADD_OUTPUTTER outputterID outputterLibPath
			std::wstring outputterIDStr;
			std::wstring outputterLibPathStr;
			strStream >> outputterIDStr;

			// Dealing with spaces in job lib path
			std::wstring outputterLibPathPartStr;
			while (strStream.eof() != true) {
				if (outputterLibPathPartStr != L"") {
					outputterLibPathStr += L" ";
				}
				strStream >> outputterLibPathPartStr;
				outputterLibPathStr += outputterLibPathPartStr;
			}
			try {
				fuzzingManager.addOutputter(outputterIDStr, outputterLibPathStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_RMOUTPUTTER) || (firstWord == MAINCFG_RMOUTPUTTER_LC)) {
			//Format: RM_OUTPUTTER outputterID
			std::wstring outputterIDStr;
			strStream >> outputterIDStr;
			try {
				fuzzingManager.removeOutputter(outputterIDStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_INITOUTPUTTER) || (firstWord == MAINCFG_INITOUTPUTTER_LC))) {
			//Format: INIT_OUTPUTTER outputterID cfgFilePath
			std::wstring outputterIDStr;
			std::wstring cfgFilePathStr;
			strStream >> outputterIDStr;

			// Dealing with spaces in job lib path
			std::wstring cfgFilePathPartStr;
			while (strStream.eof() != true) {
				if (cfgFilePathPartStr != L"") {
					cfgFilePathStr += L" ";
				}
				strStream >> cfgFilePathPartStr;
				cfgFilePathStr += cfgFilePathPartStr;
			}
			try {
				fuzzingManager.initOutputter(outputterIDStr, cfgFilePathStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_DEINITOUTPUTTER) || (firstWord == MAINCFG_DEINITOUTPUTTER_LC)) {
			//Format: DEINIT_OUTPUTTER outputterID
			std::wstring outputterIDStr;
			strStream >> outputterIDStr;
			try {
				fuzzingManager.deinitOutputter(outputterIDStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}

	// Links-related commands
		else if ((firstWord == MAINCFG_LINKOUTPUTTER) || (firstWord == MAINCFG_LINKOUTPUTTER_LC)) {
			//Format: LINK fuzzerID outputterID
			std::wstring fuzzerIDStr;
			std::wstring outputterIDStr;
			strStream >> fuzzerIDStr;
			strStream >> outputterIDStr;
			try {
				fuzzingManager.addFuzzerOutputter(fuzzerIDStr, outputterIDStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_UNLINKOUTPUTTER) || (firstWord == MAINCFG_UNLINKOUTPUTTER_LC)) {
			//Format: UNLINK fuzzerID outputterID
			std::wstring fuzzerIDStr;
			std::wstring outputterIDStr;
			strStream >> fuzzerIDStr;
			strStream >> outputterIDStr;
			try {
				fuzzingManager.removeFuzzerOutputter(fuzzerIDStr, outputterIDStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_LINKLOGGER) || (firstWord == MAINCFG_LINKLOGGER_LC)) {
			//Format: LINK fuzzerID outputterID
			std::wstring fuzzerIDStr;
			std::wstring loggerIDStr;
			strStream >> fuzzerIDStr;
			strStream >> loggerIDStr;
			try {
				fuzzingManager.addFuzzerLogger(fuzzerIDStr, loggerIDStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}
		else if ((firstWord == MAINCFG_UNLINKLOGGER) || (firstWord == MAINCFG_UNLINKLOGGER_LC)) {
			//Format: UNLINK fuzzerID outputterID
			std::wstring fuzzerIDStr;
			std::wstring loggerIDStr;
			strStream >> fuzzerIDStr;
			strStream >> loggerIDStr;
			try {
				fuzzingManager.removeFuzzerLogger(fuzzerIDStr, loggerIDStr);
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
				continue;
			}
		}

		else {
			std::wcout << L"\t" << L"[ERROR] Unknown command" << std::endl;
		}
	}

	return EXIT_SUCCESS;
}