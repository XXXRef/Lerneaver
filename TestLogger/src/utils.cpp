#include "utils.hpp"

#include <thread>

//====================================================================================================
ExException::ExException(const std::string& par_msg) : msg(par_msg) {}

#if defined PLATFORM_WINDOWS
ExWindowsError::ExWindowsError(const std::string& par_msg) : ExException(par_msg), errorCode(GetLastError()) {
	this->msg += " error_code=" + std::to_string(this->errorCode);
}
#endif

TYPE_SIZE getCPUCoresAmount() {
	return std::thread::hardware_concurrency();
}