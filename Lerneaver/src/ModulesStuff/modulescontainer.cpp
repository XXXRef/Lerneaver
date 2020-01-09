#include "modulescontainer.hpp"

//====================================================================================================
void CModulesContainer::loadModule(const TYPE_FILESYSTEMPATH& libPath) {
	auto libIter = this->modules.find(libPath);
	if (libIter==this->modules.end()) { // Job module not loaded yet
		libIter = this->modules.insert({ libPath,{std::shared_ptr<IModuleManager>(new CLASS_MODULEMANAGER(libPath)),1} }).first;
	}
	else { //Job module aready loaded
		++(libIter->second.second);
	}
}

void CModulesContainer::unloadModule(const TYPE_FILESYSTEMPATH& libPath) {
	auto libIter = this->modules.find(libPath);
	if (libIter == this->modules.end()) {
		throw ExNoModulesFound{};
	}
	if ((--(libIter->second.second))==0) {
		this->modules.erase(libIter);
	}
}

std::shared_ptr<IModuleManager> CModulesContainer::operator[](const TYPE_FILESYSTEMPATH& libPath) {
	auto libIter = this->modules.find(libPath);
	if (libIter==this->modules.end()) {
		throw ExNoModulesFound{};
	}
	return libIter->second.first;
}
