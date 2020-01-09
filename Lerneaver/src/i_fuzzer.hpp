#ifndef HEADER_I_FUZZER
#define HEADER_I_FUZZER

#include "utils.hpp"

/**
	\brief IFuzzer interface

	Abstract class describing fuzzer interface
*/
class IFuzzer {
public:
	/**
		Initializes fuzzer object with the help of data in cfg file

		\param[in] cfgFilePath Config file path
	*/
	virtual void init(const TYPE_FILESYSTEMPATH& cfgFilePath) = 0;

	/**
		Generates fuzzing data

		\param[in] par_data Optional param data
	*/
	virtual ByteBlob gen(const ByteBlob& par_data = ByteBlob{}) = 0;

	/**
		Deinitializes fuzzer
	*/
	virtual void deinit() = 0;
};

#endif