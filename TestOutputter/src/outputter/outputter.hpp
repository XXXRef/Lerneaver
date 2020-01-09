#ifndef HEADER_OUTPUTTER
#define HEADER_OUTPUTTER

#include "i_outputter.hpp"

#include <fstream>
#include <mutex>

class COutputter: public IOutputter {
	std::ofstream ofStream;
	std::mutex m;

public:
	void init(const TYPE_FILESYSTEMPATH& cfgFilePath) override;
	void output(const ByteBlob& par_data = ByteBlob{}) override;
	void deinit() override;
};

#endif
