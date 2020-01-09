#ifndef HEADER_FUZZER
#define HEADER_FUZZER

#include "i_fuzzer.hpp"

class CFuzzer: public IFuzzer {
public:
	void init(const TYPE_FILESYSTEMPATH& cfgFilePath) override;
	ByteBlob gen(const ByteBlob& par_data = ByteBlob{}) override;
	void deinit() override;
};

#endif