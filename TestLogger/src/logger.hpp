#ifndef HEADER_LOGGER
#define HEADER_LOGGER

#include "i_logger.hpp"

#include <fstream>
#include <mutex>

class CLogger: public ILogger{
	std::ofstream ofStream;
	std::mutex m;

public:
	void init(const config::TYPE_CONFIGID &cfgFileID);
	void log(const ByteBlob& par_data = ByteBlob{}, const ByteBlob& par_metadata = ByteBlob{});
	void deinit();
};

#endif