#ifndef HEADER_LOGGER
#define HEADER_LOGGER

#include "i_logger.hpp"

#include <fstream>
#include <mutex>

class CLogger: public ILogger{
	std::ofstream ofStream;
	std::mutex m;

public:
	void init(const TYPE_FILESYSTEMPATH& cfgFilePath);
	void log(const ByteBlob& par_data = ByteBlob{}, const ByteBlob& par_metadata = ByteBlob{});
	void deinit();
};

#endif