#include "fuzzer.hpp"

void CFuzzer::init(const TYPE_FILESYSTEMPATH& cfgFilePath) {
	
}

ByteBlob CFuzzer::gen(const ByteBlob& par_data) {
	return{0xDE,0xAD,0xBE,0xEF};
}

void CFuzzer::deinit() {
	
}