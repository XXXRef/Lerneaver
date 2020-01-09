#ifndef HEADER_UTILS
#define HEADER_UTILS

#include <vector>
#include <string>
#include <exception>
#include <mutex>

#include "types.hpp"

//====================================================================================================
using ByteBlob = std::vector<TYPE_BYTE>;

//====================================================================================================
template<class T> class CSharedVar {
protected:
	T var;
	std::mutex m;

public:
	CSharedVar(const T& obj = T{}) : var(obj) {}

	void CSharedVar::set(const T& obj) {
		this->m.lock();
		this->var = obj;
		this->m.unlock();
	}

	T CSharedVar::get() {
		this->m.lock();
		T tmpObj = this->var;
		this->m.unlock();
		return tmpObj;
	}
};

//====================================================================================================
class ExException : public std::exception {
public:
	std::string msg;
	ExException(const std::string& par_msg);
};

//====================================================================================================
#if defined PLATFORM_WINDOWS
#include <windows.h>
class ExWindowsError : public ExException {
public:
	DWORD errorCode;

	ExWindowsError(const std::string& par_msg);
	virtual ~ExWindowsError() {}
};
#endif

//====================================================================================================
TYPE_SIZE getCPUCoresAmount();

//Conversion
//====================================================================================================
std::string convertWStr2Str(const std::wstring& wstr);

//User utils
//====================================================================================================
void printFuzzerID(TYPE_FUZZERID fuzzerID, std::ostream& outStream) {
	
}

#endif