#pragma once
#include <string>
#include <unordered_map>
#include <bitset>

class BlockFilter {
private:
	std::unordered_map<unsigned int, std::string> targetBlocks; // Map of block ID and name of blocks to scan for
	std::bitset<65536> targetBits;
	std::bitset<65536> seenBits;

public:
	BlockFilter(const std::string& filterJSONFile);

};