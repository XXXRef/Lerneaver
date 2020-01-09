#include "outputter.hpp"

#include <stdio.h>

void COutputter::init(const TYPE_FILESYSTEMPATH& cfgFilePath) {
	//Read output file name from cfg file
	std::ifstream cfgFileIfStream(cfgFilePath, std::ios::in);
	std::string outputFilePath;
	std::getline(cfgFileIfStream,outputFilePath);
	cfgFileIfStream.close();

	//auto hFile = fopen(cfgFilePath.c_str(), "wb");
	this->ofStream.open(outputFilePath, std::ios::out | std::ios::binary | std::ios::app);
}

void COutputter::output(const ByteBlob& par_data) {
	this->m.lock();
	this->ofStream.write((const char*)&par_data[0],par_data.size());
	this->m.unlock();
}

void COutputter::deinit() {
	this->ofStream.close();
}