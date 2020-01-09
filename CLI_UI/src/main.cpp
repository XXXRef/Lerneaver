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

std::map<CFuzzingManager::EWorkerState, std::wstring> statesNames = {
	{ CFuzzingManager::EWorkerState::VANILLA, L"VANILLA"},
	{ CFuzzingManager::EWorkerState::INITIALIZED, L"INITIALIZED" },
	{ CFuzzingManager::EWorkerState::PLAYING, L"PLAYING" },
	{ CFuzzingManager::EWorkerState::STOPPED, L"STOPPED" },
	{ CFuzzingManager::EWorkerState::ENDED, L"ENDED" },
	{ CFuzzingManager::EWorkerState::DEINITIALIZED, L"DEINITIALIZED" },
	{ CFuzzingManager::EWorkerState::INTERRUPTED, L"INTERRUPTED" }
};

//Usage: FuzzingFramework cfg_filePath. cfg_filePath implied to be wide string
#if defined PLATFORM_WINDOWS
int wmain(int argc, wchar_t** argv) {
#elif defined PLATFORM_UNIX
int main(int argc, char** argv) {
#endif

	CFuzzingManager fuzzingManager;

	// Handle user input
	std::wstring userInputString;
	while (true) {
		std::wcout << "> ";
		std::getline(std::wcin, userInputString);
		std::wstringstream strStream(userInputString);
		std::wstring firstWord;
		strStream >> firstWord;
		if ((firstWord == L"EXIT") || (firstWord == L"exit")) {
			// Format: "EXIT"
			break;
		}

		if ((firstWord == L"LS") || (firstWord == L"ls")) {
			// Format: "LS"
			std::vector<std::tuple<TYPE_FUZZERID, CFuzzingManager::EWorkerState, std::vector<TYPE_LOGGERID>, std::vector<TYPE_OUTPUTTERID> >> state;
			try {
				state=fuzzingManager.getState();
			}
			catch (ExException e) {
				std::cout << "[ERROR] " << e.msg << std::endl;
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
		else if ((firstWord == L"ADD_FUZZER") || (firstWord == L"add_fuzzer")) {
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
		else if ((firstWord == L"RM_FUZZER") || (firstWord == L"rm_fuzzer")) {
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
		else if ((firstWord == L"INIT_FUZZER") || (firstWord == L"init_fuzzer")) {
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
		else if ((firstWord == L"DEINIT_FUZZER") || (firstWord == L"deinit_fuzzer")) {
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
		else if ((firstWord == L"PLAY") || (firstWord == L"play")) {
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
		else if ((firstWord == L"STOP") || (firstWord == L"stop")) {
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
		else if ((firstWord == L"ADD_LOGGER") || (firstWord == L"add_logger")) {
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
		else if ((firstWord == L"RM_LOGGER") || (firstWord == L"rm_logger")) {
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
		else if ((firstWord == L"INIT_LOGGER") || (firstWord == L"init_logger")) {
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
		else if ((firstWord == L"DEINIT_LOGGER") || (firstWord == L"deinit_logger")) {
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
		else if ((firstWord == L"ADD_OUTPUTTER") || (firstWord == L"add_outputter")) {
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
		else if ((firstWord == L"RM_OUTPUTTER") || (firstWord == L"rm_outputter")) {
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
		else if ((firstWord == L"INIT_OUTPUTTER") || (firstWord == L"init_outputter")) {
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
		else if ((firstWord == L"DEINIT_OUTPUTTER") || (firstWord == L"deinit_outputter")) {
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
		else if ((firstWord == L"LINK_OUTPUTTER") || (firstWord == L"link_outputter")) {
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
		else if ((firstWord == L"UNLINK_OUTPUTTER") || (firstWord == L"unlink_outputter")) {
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
		else if ((firstWord == L"LINK_LOGGER") || (firstWord == L"link_logger")) {
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
		else if ((firstWord == L"UNLINK_LOGGER") || (firstWord == L"unlink_logger")) {
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