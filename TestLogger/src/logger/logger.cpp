#include "logger.hpp"

void CLogger::init(const TYPE_FILESYSTEMPATH& cfgFilePath) {
	//Read output file name from cfg file
	std::ifstream cfgFileIfStream(cfgFilePath, std::ios::in);
	std::string outputFilePath;
	std::getline(cfgFileIfStream, outputFilePath);
	cfgFileIfStream.close();

	//auto hFile = fopen(cfgFilePath.c_str(), "wb");
	this->ofStream.open(outputFilePath, std::ios::out | std::ios::binary | std::ios::app);
}

void CLogger::log(const ByteBlob& par_data, const ByteBlob& par_metadata) {
	this->m.lock();
	this->ofStream.write((const char*)&par_data[0], par_data.size());
	this->m.unlock();
}

void CLogger::deinit() {
	this->ofStream.close();
}