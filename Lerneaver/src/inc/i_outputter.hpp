#ifndef HEADER_I_OUTPUTTER
#define HEADER_I_OUTPUTTER

#include "types.hpp"

/**
	\brief IOutputter interface

	Abstract class describing outputter interface
*/
class IOutputter {
public:
	/**
	Initializes outputter object with the help of data in cfg

	\param[in] cfgFilePath Config ID
	*/
	virtual void init(const config::TYPE_CONFIGID &cfgFilePath) = 0;

	/**
		Sends fuzzer buffer to right place

		\param[in] par_data fuzzing data buffer
	*/
	virtual void output(const ByteBlob &par_data = ByteBlob{}) = 0;//TODO par_data may not be empty => def arg is redundant

	/**
		Deinitializes ID
	*/
	virtual void deinit() = 0;
};

#endif
