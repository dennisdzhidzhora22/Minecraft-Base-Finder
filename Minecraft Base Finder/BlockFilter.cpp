#include "BlockFilter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

/*
*********************
Private methods below
*********************
*/

void BlockFilter::loadFile(const std::string& filterFile) {
	if (std::filesystem::exists(filterFile)) {
		std::ifstream file(filterFile);

		std::string line, name;
		unsigned int id, subid, addid, weight;

		while (std::getline(file, line)) {
			if (line.empty() || line[0] == '#') {
				continue;
			}

			std::replace(line.begin(), line.end(), ',', ' ');
			std::stringstream ss(line);

			ss >> name >> id >> subid >> addid >> weight;

			unsigned int fullID = (id << 8) + (addid << 4) + subid;
			targetBlocks.insert({ fullID, name });
			targetBits[fullID] = true;
		}
	}
	else {
		std::cout << "Warning: Filter file not found at file \"" << filterFile << "\"\n";
	}
}

/*
*********************
Public methods below
*********************
*/

BlockFilter::BlockFilter() {

}

void BlockFilter::loadDefault() {
	loadFile("DefaultBlockFilter.txt");
}

void BlockFilter::loadCustom(const std::string& filterFile) {
	loadFile(filterFile);
}

std::string BlockFilter::getNameFromID(unsigned int ID) const {
	return targetBlocks.at(ID);
}

bool BlockFilter::isTarget(unsigned int ID) const {
	return targetBits[ID];
}
