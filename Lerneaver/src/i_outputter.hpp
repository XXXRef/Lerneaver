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
	Initializes outputter object with the help of data in cfg file

	\param[in] cfgFilePath Config file path
	*/
	virtual void init(const TYPE_FILESYSTEMPATH& cfgFilePath) = 0;

	/**
		Sends fuzzer buffer to right place

		\param[in] par_data fuzzing data buffer
	*/
	virtual void output(const ByteBlob& par_data = ByteBlob{}) = 0;

	/**
		Deinitializes outputter
	*/
	virtual void deinit() = 0;
};

#endif
