#pragma once
#include <string>
#include <unordered_map>
#include <bitset>

class BlockFilter {
private:
	std::unordered_map<unsigned int, std::string> targetBlocks; // Map of block ID and name of blocks to scan for
	std::bitset<65536> targetBits;
	bool ready = false;

	void loadFile(const std::string& filterFile);

public:
	BlockFilter();
	void loadDefault();
	void loadCustom(const std::string& filterFile);

	std::string getNameFromID(unsigned int ID) const;
	bool isTarget(unsigned int ID) const;
};