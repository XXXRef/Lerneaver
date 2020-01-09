#ifndef HEADER_I_LOGGER
#define HEADER_I_LOGGER

#include "utils.hpp"

/**
\brief ILogger interface

Abstract class describing logger interface
*/
class ILogger {
public:
	/**
	Initializes logger object with the help of data in cfg file

	\param[in] cfgFilePath Config file path
	*/
	virtual void init(const TYPE_FILESYSTEMPATH& cfgFilePath) = 0;

	/**
	Logs data blob from fuzzer optionally using metadata

	\param[in] par_data fuzzing data buffer
	\param[in] par_metadata fuzzing data buffer (optional)
	*/
	virtual void log(const ByteBlob& par_data = ByteBlob{}, const ByteBlob& par_metadata = ByteBlob{}) = 0;

	/**
	Deinitializes logger
	*/
	virtual void deinit() = 0;
};

#endif