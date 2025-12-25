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
#define CHUNK 16384

struct ChunkLocation { // Could be changed to use 4 bytes, may lead to less cache misses and better performance
	unsigned int offset;
	unsigned int sectorCount;
};

class Region {
private:
	//unsigned char* header = new unsigned char[8192]();
	std::vector<unsigned char> header;
	//int** chunkInfo = new int* [1024]; // Array of arrays which hold offset and sector size for each chunk
	std::vector<ChunkLocation> chunkInfo; // Vector of ChunkLocation structs holding offset and sector size for each chunk
	std::vector<std::vector<unsigned char>> chunkDataCompressed;
	std::vector<std::vector<unsigned char>> chunkDataUncompressed;
	//std::vector<int> chunkDataUncompressedLengths;
	nbt::tag_compound tagComp;
	std::ifstream iFile;
	std::ofstream oFile;
	std::string filePath = "";
	/*std::vector< std::pair<int, int> > targetBlocks;*/ // Vector of block id and sub-id for blocks to scan for
	std::map<unsigned int, std::string> targetBlocks;

	// Holds number and variety of detected blocks for each chunk, then x and z coordinate
	//std::pair<int, int> chunkScores[1024] = { {0, 0} };
	int chunkScoreCount[1024] = { 0 };
	int chunkScoreVariety[1024] = { 0 };
	//std::pair<int, int> chunkPos[1024] = { {0, 0} };
	int chunkX[1024] = { 0 };
	int chunkZ[1024] = { 0 };

	// Doubles the size of a dynamic array
	template <typename T>
	void resizeDynArr(T*& arr, int& size);

	// Copies header data into array
	void CopyHeaderData(std::vector<unsigned char>& header1, const std::string& file, std::ifstream& inFile);

	// Reads and organizes data from header array into chunkInfo array
	void ParseHeader(std::vector<unsigned char>& header2, std::vector<ChunkLocation>& chunkInfo);

	// Writes a single chunk's compressed data into a vector. Also sets vector's size.
	void readChunk(const int offset, const int sectorCount, std::vector<unsigned char>& chunkDataCompressed, std::ifstream& inFile);

	void uncompressChunk(std::vector<unsigned char>& chunkDataCompressed, std::vector<unsigned char>& chunkDataUncompressed);

	// Returns number and variety of detected blocks in given section
	std::pair<int, int> chunkScore(nbt::tag_list sections);

	void readNBT(std::vector<unsigned char>& chunkDataUncompressed, int chunkNumber);

	//Real public:
public:
	Region();

	Region(const std::string& file);

	void setFilePath(const std::string& file);

	void startTask();

	/*std::pair<int, int> getChunkScore(int i);*/

	/*std::pair<int, int> getChunkPos(int i);*/

	int getChunkScoreCount(int i);
	int getChunkScoreVariety(int i);
	int getChunkX(int i);
	int getChunkZ(int i);

	~Region();
};

#endif