#ifndef HEADER_I_LOGGER
#define HEADER_I_LOGGER

#include "utils.hpp"
#include "config.hpp"

/**
\brief ILogger interface

Abstract class describing logger interface
*/
class ILogger {
public:
	/**
	Initializes logger object with the help of data in cfg file

	\param[in] cfgFileID Config ID
	*/
	virtual void init(const config::TYPE_CONFIGID &cfgFileID) = 0;

	/**
	Logs data blob from fuzzer optionally using metadata

	\param[in] par_data fuzzing data buffer
	\param[in] par_metadata fuzzing data buffer (optional)
	*/
	virtual void log(const ByteBlob &par_data = ByteBlob{}, const ByteBlob &par_metadata = ByteBlob{}) = 0; //TODO what the fuck is metadata?

	/**
	Deinitializes logger
	*/
	virtual void deinit() = 0;
};

#endif