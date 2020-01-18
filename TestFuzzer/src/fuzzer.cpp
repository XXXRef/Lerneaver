#include "fuzzer.hpp"

void CFuzzer::init(const config::TYPE_CONFIGID &cfgFileID) {
	
}

ByteBlob CFuzzer::gen(const ByteBlob &par_data) {
	return{0xDE,0xAD,0xBE,0xEF};
}

void CFuzzer::deinit() {
	
}
