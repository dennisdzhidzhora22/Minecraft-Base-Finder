#include "Region.h"

/*
*********************
Private methods below
*********************
*/

// Doubles the size of a dynamic array
template <typename T>
void Region::resizeDynArr(T*& arr, int& size)
{
	int newSize = size * 2;
	T* newArray = new T[newSize];

	for (int i = 0; i < newSize; i++)
		newArray[i] = 0; // If trimming of empty space is needed, this could be changed to something else to leave an "imprint" of where data ends
	for (int i = 0; i < size; i++)
		newArray[i] = arr[i];

	size = newSize;
	delete[] arr;
	arr = newArray;
}

// Copies header data into array
void Region::copyHeaderData(std::vector<unsigned char>& header1, const std::string& file, std::ifstream& inFile)
{
	inFile.open(file, std::ios::binary);

	//int i = 0;

	if (inFile.is_open())
	{
		/*while (inFile.good() && i < 8192)
		{
			header1[i] = inFile.get();
			i++;
		}*/

		inFile.read(reinterpret_cast<char*>(header1.data()), 8192);
	}
	else
		std::cout << "Error: Could not open file " << file << "\n";
}

// Reads and organizes data from header array into chunkInfo array
void Region::parseHeader(std::vector<unsigned char>& header2, std::vector<ChunkLocationHeader>& chunkInfo)
{
	unsigned int offset = 0; //Offset from start of file in 4KiB sections.
	unsigned int temp = 0;
	unsigned int sectorCount = 0; //Number of 4KiB sectors used, rounded up.

	for (int i = 0; i < 4096; i += 4) // Regions may have less than 1024 chunks, so this will be changed to a while loop
		// NVM non-blank chunks aren't always at the beginning
	{
		temp = header2[i];
		temp = temp << 16;
		offset = temp;
		//std::cout << std::hex << temp << std::endl;
		//std::cout << std::hex << offset << std::endl;

		temp = header2[i + 1];
		temp = temp << 8;
		offset = offset | temp;
		//std::cout << std::hex << temp << std::endl;
		//std::cout << std::hex << offset << std::endl;

		temp = header2[i + 2];
		offset = offset | temp;
		//std::cout << std::hex << temp << std::endl;
		//std::cout << std::hex << offset << std::endl;

		sectorCount = header2[i + 3];

		//assert(i < 4096);
		chunkInfo[i / 4].offset = offset;
		chunkInfo[i / 4].sectorCount = sectorCount;
		//if (i / 4 == 0)
		//{
		//	//std::cout << std::hex << offset << "THIS IS THE OFFSET" << std::endl;
		//}
		//std::cout << static_cast<int>(sectorCount) << " - SectorCount" << std::endl;
		//std::cout << "Read to " << std::hex << i << std::dec << std::endl;
	}
	//std::cout << std::dec << chunkInfo[0][3] << std::endl << "test123" << std::endl;
}

// Writes a single chunk's compressed data into a vector. Also sets vector's size.
void Region::readChunk(const int offset, const int sectorCount, std::vector<unsigned char>& chunkDataCompressed, std::ifstream& inFile)
{
	if (offset == 0 && sectorCount == 0) { // If chunk isn't present in region
		chunkDataCompressed.clear();
		return;
	}

	// Move to starting position of payload based on offset
	inFile.seekg(offset * 4096);

	int length = 0;
	int temp = 0;
	unsigned char compressionType = 0;

	temp = inFile.get();
	temp = temp << 24;
	length = temp;

	temp = inFile.get();
	temp = temp << 16;
	length = length | temp;

	temp = inFile.get();
	temp = temp << 8;
	length = length | temp;

	temp = inFile.get();
	length = length | temp;

	compressionType = inFile.get();

	if (compressionType != 2) {
		std::cout << "Error: Compression type is not Zlib\n";
	}

	//std::cout << "Length of compressed chunk data is " << length << /*" OR " << sectorCount <<*/ " bytes.\n" <<
	//	"Compression type of chunk data is " << int(compressionType) << ".\n";

	int payloadLen = length - 1;

	chunkDataCompressed.resize(payloadLen);
	inFile.read(reinterpret_cast<char*>(chunkDataCompressed.data()), payloadLen);

	//for (int remaining = length - 1; remaining > 0; remaining--)
	//{
	//	unsigned char tempChar = inFile.get();
	//	//chunkDataCompressed[4929 - remaining] = tempChar; // This 4929 looks weird, may need to be changed
	//	chunkDataCompressed[payloadLen - remaining] = tempChar;
	//}

	//std::cout << inFile.tellg() << " - Current Position in File, peek = " << inFile.peek() << "\n";

}

void Region::uncompressChunk(std::vector<unsigned char>& chunkDataCompressed, std::vector<unsigned char>& chunkDataUncompressed) {
	/*unsigned char* chunkDataCompressed1 = new unsigned char[chunkDataCompressed.size() + 1];
	for (int i = 0; i < chunkDataCompressed.size(); i++)
	{
		chunkDataCompressed1[i] = chunkDataCompressed[i];
	}*/

	/*uLong destLen = chunkDataCompressed.size();
	chunkDataUncompressed = new unsigned char[destLen]();
	int len = destLen;
	chunkDataUncompressedLength = len;*/

	uLong destLen = 131072; // 128 KiB - overallocate to avoid resizing
	chunkDataUncompressed.resize(destLen);
	//int len = destLen;

	int ret = uncompress(chunkDataUncompressed.data(), &destLen, chunkDataCompressed.data(), chunkDataCompressed.size());

	while (ret == Z_BUF_ERROR && ret != Z_OK) // While not enough space in destination, resize and try again
	{
		if (ret == Z_MEM_ERROR)
		{
			//std::cout << "\nZ_MEM_ERROR\n";
			break;
		}
		//resizeDynArr(chunkDataUncompressed, len);
		//destLen = len;
		//chunkDataUncompressedLength = len;

		chunkDataUncompressed.resize(destLen * 2);
		destLen = chunkDataUncompressed.size();

		//std::cout << "Increased size\n";
		//if (chunkDataUncompressed == nullptr) {
		//	//std::cout << "Something went wrong...\n";
		//}
		//else
		//	//std::cout << "Written successfully!\n";
		//	ret = uncompress(chunkDataUncompressed.data(), &destLen, chunkDataCompressed.data(), chunkDataCompressed.size());
		ret = uncompress(chunkDataUncompressed.data(), &destLen, chunkDataCompressed.data(), chunkDataCompressed.size());
	}
	chunkDataUncompressed.resize(destLen);
	chunkDataUncompressed.shrink_to_fit();
	//chunkDataUncompressedLength = destLen + 5; // Not sure if any data is being cut off, so added 5 just in case

	//delete[] chunkDataCompressed1;
}

// Returns number and variety of detected blocks in given section
std::pair<int, int> Region::getChunkScore(nbt::tag_list sections) {
	// For now, the add array will be ignored

	int count = 0;
	int variety = 0;

	seenBits.reset();

	/*std::map<unsigned int, std::string> tBlocks;
	for (std::map<unsigned int, std::string>::iterator it = targetBlocks.begin(); it != targetBlocks.end(); ++it) {
		tBlocks.insert({ it->first, it->second });
	}*/

	for (nbt::tag_list::iterator it = sections.begin(); it != sections.end(); ++it) {
		nbt::tag_compound section = (*it).as<nbt::tag_compound>();
		nbt::tag_byte_array blocks = section["Blocks"].as<nbt::tag_byte_array>();
		nbt::tag_byte_array data = section["Data"].as<nbt::tag_byte_array>();

		if (!section.has_key("Blocks") || !section.has_key("Data")) {
			continue;
		}

		for (int i = 0; i < 4096; i++) {
			unsigned char id = blocks[i];
			unsigned char subid = 0;

			if (i % 2 == 0) { // Even-numbered blocks
				unsigned char temp = data[i / 2];
				//subid = temp >> 4;
				subid = temp & 0x0F;
				//if (id != 0 || subid != 0) {
					//std::cout << std::hex << static_cast<int>(id) << " : " << static_cast<int>(subid) << std::dec << "\n";
				//}
			}
			else { // Odd-numbered blocks
				unsigned char temp = data[i / 2];
				//temp = temp << 4;
				//temp = temp >> 4;
				subid = temp & 0xF0;
				subid = subid >> 4;
				//if (id != 0 || subid != 0) {
					//std::cout << std::hex << static_cast<int>(id) << " : " << static_cast<int>(subid) << std::dec << "\n";
				//}
			}

			// Exclude most common blocks
			if (id == 0 || (id == 1 && subid == 0) || id == 2 || id == 3 || id == 8 || id == 9 || id == 12 || id == 18 || id == 24) {
				continue;
			}

			//std::pair<unsigned int, unsigned int> block = { id, subid };

			// If found in target blocks
			// This is very very slow, consider changing the vector to a map and 
			// converting the two id's into 1 number, then searching
			/*if (std::binary_search(targetBlocks.begin(), targetBlocks.end(), block)) {
				count++;

				if (std::binary_search(tBlocks.begin(), tBlocks.end(), block)) {
					tBlocks.erase(std::find(tBlocks.begin(), tBlocks.end(), block));
					variety++;
				}
			}*/

			/*if (targetBlocks.find((block.first << 8) + block.second) != targetBlocks.end()) {
				count++;

				if (tBlocks.find((block.first << 8) + block.second) != tBlocks.end()) {
					tBlocks.erase((block.first << 8) + block.second);
					variety++;
				}
			}*/

			unsigned int fullID = (id << 8) + subid;
			if (targetBits[fullID] == true) {
				count++;

				if (seenBits[fullID] == false) {
					seenBits[fullID] = true;
					variety++;
				}
			}
		}
	}

	//std::cout << "\ncount, variety:	" << count << ":" << variety << "\n";

	return { count, variety };
}

void Region::readNBT(std::vector<unsigned char>& chunkDataUncompressed, int chunkNumber) {
	/*std::string s(reinterpret_cast<char*>(chunkDataUncompressed.data()), chunkDataUncompressed.size());
	std::istringstream stream(s);*/

	char* begin = reinterpret_cast<char*>(chunkDataUncompressed.data());
	char* end = begin + chunkDataUncompressed.size();

	membuf cbuf(begin, end); // Zero-copy buffer view of chunkDataUncompressed
	std::istream stream(&cbuf);

	nbt::io::stream_reader data(stream);
	std::pair<std::string, std::unique_ptr<nbt::tag_compound>> p = data.read_compound();
	//std::cout << "\n\n" << p.first << "\n\ndid it work?\n";

	nbt::tag_compound root = *p.second;

	//nbt::tag_compound LevelComp = Level.as<nbt::tag_compound>();
	// or
	//nbt::tag_compound LevelComp = nbt::value(root["Level"]).as<nbt::tag_compound>();
	// or
	nbt::tag_compound LevelComp = root["Level"].as<nbt::tag_compound>();

	//nbt::tag_int xPos = LevelComp["xPos"].as<nbt::tag_int>();
	//nbt::tag_int zPos = LevelComp["zPos"].as<nbt::tag_int>();

	int& xPos = LevelComp["xPos"].as<nbt::tag_int>();
	int& zPos = LevelComp["zPos"].as<nbt::tag_int>();

	//DEBUGstd::cout << "xPos: " << xPos << "\nzPos: " << zPos << "\n";

	nbt::tag_list SectionsList = LevelComp["Sections"].as<nbt::tag_list>();

	std::pair<int, int> totalscore = { 0, 0 };

	totalscore = getChunkScore(SectionsList);

	//for (nbt::tag_list::iterator it = SectionsList.begin(); it != SectionsList.end(); ++it) {
	//	std::cout << "Entry: " << (*it) << " " << typeid((*it).get()).name() << "\n";

	//	nbt::tag_compound section = (*it).as<nbt::tag_compound>();
	//	nbt::tag_byte_array blocks = section["Blocks"].as<nbt::tag_byte_array>();
	//	nbt::tag_byte_array data = section["Data"].as<nbt::tag_byte_array>();

	//	nbt::tag_byte_array add;
	//	if (section.has_key("Add")) {
	//		add = section["Add"].as<nbt::tag_byte_array>();
	//	}

	//	//std::pair<int, int> score = chunkScore(blocks, data, add);
	//	totalscore.first += score.first;
	//	totalscore.second += score.second;

	//	//for (nbt::tag_byte_array::iterator it2 = (*it).begin())
	//}

	//DEBUGstd::cout << "\n" << totalscore.first << " | " << totalscore.second << "\n";

	//std::pair<int, int> blocks[4096] = { {0, 0} };

	//nbt::tag* ptr = p.second;

	//chunkScores[chunkNumber] = totalscore;
	//chunkPos[chunkNumber] = { xPos, zPos };
	/*chunkScoreCount[chunkNumber] = totalscore.first;
	chunkScoreVariety[chunkNumber] = totalscore.second;
	chunkX[chunkNumber] = xPos;
	chunkZ[chunkNumber] = zPos;*/

	chunkScores[chunkNumber].scoreCount = totalscore.first;
	chunkScores[chunkNumber].scoreVariety = totalscore.second;
	chunkPositions[chunkNumber].xPos = xPos;
	chunkPositions[chunkNumber].zPos = zPos;
}

/*
*********************
Public methods below
*********************
*/

Region::Region() : header(8192), chunkInfo(1024), chunkDataCompressed(1024), chunkDataUncompressed(1024), chunkScores(1024), chunkPositions(1024) {
	/*for (int i = 0; i < 1024; i++)
		chunkInfo[i] = new int[4];*/

	//header.resize(8192);
	//chunkDataCompressed.resize(1024);
	//chunkDataUncompressed.resize(1024);
	//chunkDataUncompressedLengths.resize(1024);

	targetBlocks = { {(137 << 8) + 0, "Command Block"}, {(255 << 8) + 0, ""}, {(166 << 8) + 0, ""}, {(58 << 8) + 0, ""},
					{(23 << 8) + 0, ""}, {(158 << 8) + 0, ""}, {(25 << 8) + 0, ""}, {(84 << 8) + 0, ""},
					{(116 << 8) + 0, ""}, {(117 << 8) + 0, ""}, {(145 << 8) + 0, ""}, {(145 << 8) + 1, ""},
					{(145 << 8) + 2, ""}, {(154 << 8) + 0, ""}, {(138 << 8) + 0, ""}, {(130 << 8) + 0, ""},
					{(55 << 8) + 0, ""}, {(123 << 8) + 0, ""}, {(124 << 8) + 0, ""}, {(33 << 8) + 0, ""},
					{(35 << 8) + 0, ""}, {(34 << 8) + 0, ""}, {(29 << 8) + 0, ""}, {(69 << 8) + 0, ""}, //35 was meant to replace a duplicate 33
					{(143 << 8) + 0, ""}, {(77 << 8) + 0, ""}, {(70 << 8) + 0, ""}, {(72 << 8) + 0, ""},
					{(147 << 8) + 0, ""}, {(148 << 8) + 0, ""}, {(93 << 8) + 0, ""}, {(94 << 8) + 0, ""},
					{(149 << 8) + 0, ""}, {(150 << 8) + 0, ""}, {(218 << 8) + 0, ""}, {(46 << 8) + 0, ""},
					{(63 << 8) + 0, ""}, {(68 << 8) + 0, ""}, {(140 << 8) + 0, ""}, {(26 << 8) + 0, ""},
					{(71 << 8) + 0, ""}, {(65 << 8) + 0, ""}, {(229 << 8) + 0, ""}, {(107 << 8) + 0, ""},
					{(183 << 8) + 0, ""}, {(184 << 8) + 0, ""}, {(185 << 8) + 0, ""}, {(186 << 8) + 0, ""},
					{(187 << 8) + 0, ""}, {(101 << 8) + 0, ""} };
	//std::sort(targetBlocks.begin(), targetBlocks.end());

	for (auto const& pair : targetBlocks) {
		targetBits[pair.first] = true;
	}
}

Region::Region(const std::string& file) : header(8192), chunkInfo(1024), chunkDataCompressed(1024), chunkDataUncompressed(1024), chunkScores(1024), chunkPositions(1024) {
	/*for (int i = 0; i < 1024; i++)
		chunkInfo[i] = new int[4];*/

	//chunkDataCompressed.resize(1024);
	//chunkDataUncompressed.resize(1024);
	//chunkDataUncompressedLengths.resize(1024);

	filePath = file;
	//CopyHeaderData(header, filePath, iFile);
	/*for (int i = 0; i < 8192; i++)
		std::cout << std::hex << int(header[i]) << " ";*/

		// Put IDs in order and maybe implement binary search for it
	targetBlocks = { {(137 << 8) + 0, "Command Block"}, {(255 << 8) + 0, ""}, {(166 << 8) + 0, ""}, {(58 << 8) + 0, ""},
					{(23 << 8) + 0, ""}, {(158 << 8) + 0, ""}, {(25 << 8) + 0, ""}, {(84 << 8) + 0, ""},
					{(116 << 8) + 0, ""}, {(117 << 8) + 0, ""}, {(145 << 8) + 0, ""}, {(145 << 8) + 1, ""},
					{(145 << 8) + 2, ""}, {(154 << 8) + 0, ""}, {(138 << 8) + 0, ""}, {(130 << 8) + 0, ""},
					{(55 << 8) + 0, ""}, {(123 << 8) + 0, ""}, {(124 << 8) + 0, ""}, {(33 << 8) + 0, ""},
					{(35 << 8) + 0, ""}, {(34 << 8) + 0, ""}, {(29 << 8) + 0, ""}, {(69 << 8) + 0, ""}, //35 was meant to replace a duplicate 33
					{(143 << 8) + 0, ""}, {(77 << 8) + 0, ""}, {(70 << 8) + 0, ""}, {(72 << 8) + 0, ""},
					{(147 << 8) + 0, ""}, {(148 << 8) + 0, ""}, {(93 << 8) + 0, ""}, {(94 << 8) + 0, ""},
					{(149 << 8) + 0, ""}, {(150 << 8) + 0, ""}, {(218 << 8) + 0, ""}, {(46 << 8) + 0, ""},
					{(63 << 8) + 0, ""}, {(68 << 8) + 0, ""}, {(140 << 8) + 0, ""}, {(26 << 8) + 0, ""},
					{(71 << 8) + 0, ""}, {(65 << 8) + 0, ""}, {(229 << 8) + 0, ""}, {(107 << 8) + 0, ""},
					{(183 << 8) + 0, ""}, {(184 << 8) + 0, ""}, {(185 << 8) + 0, ""}, {(186 << 8) + 0, ""},
					{(187 << 8) + 0, ""}, {(101 << 8) + 0, ""} };
	//std::sort(targetBlocks.begin(), targetBlocks.end());

	for (auto const& pair : targetBlocks) {
		targetBits[pair.first] = true;
	}
}

void Region::setFilePath(const std::string& file) {
	filePath = file;
	//CopyHeaderData(header, filePath, iFile);
}

void Region::startTask() {
	if (filePath == "") {
		std::cout << "\nError: No file path available.\n";
		return;
	}

	// Header section
	copyHeaderData(header, filePath, iFile);
	parseHeader(header, chunkInfo);

	const int numOfChunks = 1024;

	// Chunks section
	for (int i = 0; i < numOfChunks; i++) {
		readChunk(chunkInfo[i].offset, chunkInfo[i].sectorCount, chunkDataCompressed[i], iFile);
		/*oFile.open("output/o" + std::to_string(i) + filePath, std::ios::binary);
		for (int j = 0; j < chunkDataCompressed[i].size(); j++) {
			oFile << chunkDataCompressed[i][j];
		}
		oFile.close();
		std::cout << "Last element in vector is " << std::hex << int(chunkDataCompressed[i].back()) << std::dec << "\n";*/
	}

	// Decompression section
	for (int i = 0; i < numOfChunks; i++) {
		if (chunkDataCompressed[i].empty()) continue;

		uncompressChunk(chunkDataCompressed[i], chunkDataUncompressed[i]);
	}
	//for (int i = 0; i < numOfChunks; i++) {
	//	oFile.open("output/uncomp/uncompChunkTest" + std::to_string(i) + ".txt", std::ios::binary);
	//	//unsigned char* uncompArr = chunkDataUncompressed[i];
	//	for (int j = 0; j < chunkDataUncompressedLengths[i]; j++) {
	//		oFile << chunkDataUncompressed[i][j];
	//		/*std::cout << "About to print a value...\n";
	//		std::cout << int(chunkDataUncompressed[0]) << "\n" << int(chunkDataUncompressed[i]) << "\n" << int(uncompArr) << "\n" << int(uncompArr == nullptr) << "\n";
	//		std::cout << "\nDid it work?\n";*/
	//	}
	//	oFile.close();
	//}
	//oFile.close();

	//std::cout << "Last element in vector is " << std::hex << int(chunkDataCompressed[0].back()) << "\n";
	/*for (int i = 0; i < 6; i++) {
		std::cout << iFile.get() << " ";
	}*/

	// NBT Reading Section
	for (int i = 0; i < numOfChunks; i++) {
		if (chunkDataUncompressed[i].empty()) continue;

		readNBT(chunkDataUncompressed[i], i);
	}
}

/*std::pair<int, int> Region::getChunkScore(int i) {
		if (i < 0 || i > 1023) {
			std::cout << "Invalid Chunk Score Range, choose an index from 0 - 1023\n";
		}
		else
			return chunkScores[i];
}*/

/*std::pair<int, int> Region::getChunkPos(int i) {
		if (i < 0 || i > 1023) {
			std::cout << "Invalid Chunk Position Range, choose an index from 0 - 1023\n";
		}
		else
			return chunkPos[i];
}*/

int Region::getChunkScoreCount(int i) {
	//return chunkScoreCount[i];
	return chunkScores[i].scoreCount;
}
int Region::getChunkScoreVariety(int i) {
	//return chunkScoreVariety[i];
	return chunkScores[i].scoreVariety;
}
int Region::getChunkX(int i) {
	//return chunkX[i];
	return chunkPositions[i].xPos;
}
int Region::getChunkZ(int i) {
	return chunkPositions[i].zPos;
}

Region::~Region() {
	iFile.close();
	oFile.close();

	//delete[] header;

	/*for (int i = 0; i < 1024; i++)
	{
		delete[] chunkInfo[i];
	}
	delete[] chunkInfo;*/

	/*for (int i = 0; i < 1024; i++)
	{
		delete[] chunkDataUncompressed[i];
	}*/
}