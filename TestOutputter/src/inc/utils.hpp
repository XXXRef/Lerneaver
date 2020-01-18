#ifndef HEADER_UTILS
#define HEADER_UTILS

#include <vector>
#include <string>
#include <exception>
#include <mutex>

#include "types.hpp"
#include "config.hpp"

//====================================================================================================
class ExEx : public std::exception {

public:
	ExEx() = default;
	virtual ~ExEx() {}
	virtual std::string getInfo() const = 0;
};

//====================================================================================================
using ByteBlob = std::vector<TYPE_BYTE>;

//====================================================================================================
template<class T> class CSharedVar {
protected:
	T var;
	std::mutex m;

public:
	CSharedVar(const T &obj = T{}) : var(obj) {}//TODO noexcept?
	//TODO take care bout copy-control members
	virtual ~CSharedVar() {}

	void set(const T &obj = T{}) {
		std::lock_guard<std::mutex>(this->m);
		this->var = obj;
	}

	T get() {
		std::lock_guard<std::mutex>(this->m);
		T tmpObj = this->var;
		return tmpObj;
	}
};

//====================================================================================================
inline TYPE_SIZE getCPUCoresAmount() {
	return std::thread::hardware_concurrency();
}

//Conversion stuff
//====================================================================================================
inline std::string convertWStr2Str(const std::wstring &wstr) {
	return std::string(wstr.begin(), wstr.end());
}

//User utils
//====================================================================================================
inline void printFuzzerID(const config::TYPE_FUZZERID &fuzzerID, std::ostream &outStream) {}

#endif