#ifndef REGION_
#define REGION_
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <zlib.h>
#include <io/stream_reader.h>
#include <tag_list.h>
#include <tag_primitive.h>
#include <tag_array.h>
#include <algorithm>
#include <utility>
#include <cassert>
#include <bitset>
#include <unordered_map>
#include "BlockFilter.h"
constexpr auto CHUNK = 16384;

struct membuf : std::streambuf {
	membuf(char* begin, char* end) {
		this->setg(begin, begin, end);
	}
};

struct ChunkLocationHeader { // Could be changed to use 4 bytes, may lead to less cache misses and better performance
	unsigned int offset;
	unsigned int sectorCount;
};

struct ChunkScoreInfo {
	int scoreCount;
	int scoreVariety;
};

struct ChunkPos {
	int xPos;
	int zPos;
};

class Region {
private:
	std::vector<unsigned char> header;
	std::vector<ChunkLocationHeader> chunkInfo; // Vector of ChunkLocationHeader structs holding offset and sector size for each chunk
	std::vector<std::vector<unsigned char>> chunkDataCompressed;
	std::vector<std::vector<unsigned char>> chunkDataUncompressed;
	std::ifstream iFile;
	std::ofstream oFile;
	std::string filePath = "";
	const BlockFilter* filter;
	std::bitset<65536> seenBits;

	// Holds number and variety of detected blocks for each chunk, then x and z coordinate
	std::vector<ChunkScoreInfo> chunkScores;
	std::vector<ChunkPos> chunkPositions;

	// Doubles the size of a dynamic array
	template <typename T>
	void resizeDynArr(T*& arr, int& size);

	// Copies header data into array
	void copyHeaderData(std::vector<unsigned char>& header1, const std::string& file, std::ifstream& inFile);

	// Reads and organizes data from header array into chunkInfo array
	void parseHeader(std::vector<unsigned char>& header2, std::vector<ChunkLocationHeader>& chunkInfo);

	// Writes a single chunk's compressed data into a vector.
	void readChunk(const int offset, const int sectorCount, std::vector<unsigned char>& chunkDataCompressed, std::ifstream& inFile);

	void uncompressChunk(std::vector<unsigned char>& chunkDataCompressed, std::vector<unsigned char>& chunkDataUncompressed);

	// Returns number and variety of detected blocks in given section
	std::pair<int, int> getChunkScore(nbt::tag_list sections);

	void readNBT(std::vector<unsigned char>& chunkDataUncompressed, int chunkNumber);

public:
	Region(const BlockFilter& filterRef);

	Region(const std::string& file, const BlockFilter& filterRef);

	void setFilePath(const std::string& file);

	void startTask();

	int getChunkScoreCount(int i);
	int getChunkScoreVariety(int i);
	int getChunkX(int i);
	int getChunkZ(int i);

	~Region();
};

#endif