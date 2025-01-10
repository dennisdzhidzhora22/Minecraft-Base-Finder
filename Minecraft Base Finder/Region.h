#ifndef REGION_
#define REGION_
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//bla

class Region {
private:
	unsigned char* header = new unsigned char[8192]();
	int** chunkInfo = new int* [1024]; // Array of arrays which hold offset and sector size for each chunk
	std::vector<std::vector<unsigned char>> chunkDataCompressed;
	std::ifstream iFile;
	std::ofstream oFile;
	std::string filePath = "";

	// Copies header data into array
	void CopyHeaderData(unsigned char header1[], std::string& file, std::ifstream& inFile)
	{
		inFile.open(file, std::ios::binary);

		int i = 0;

		if (inFile.is_open())
		{
			while (inFile.good() && i < 8192)
			{
				header1[i] = inFile.get();
				i++;
			}
		}
		else
			std::cout << "Could not open file.\n";
	}

	// Reads and organizes data from header array into chunkInfo array
	void ParseHeader(unsigned char header2[], int** chunkInfo)
	{
		int offset = 0; //Offset from start of file in 4KiB sections.
		int temp = 0;
		unsigned char sectorCount = 0; //Number of 4KiB sectors used, rounded up.

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
			offset = offset ^ temp;
			//std::cout << std::hex << temp << std::endl;
			//std::cout << std::hex << offset << std::endl;

			temp = header2[i + 2];
			offset = offset ^ temp;
			//std::cout << std::hex << temp << std::endl;
			//std::cout << std::hex << offset << std::endl;

			sectorCount = header2[i + 3];

			assert(i < 4096);
			chunkInfo[i / 4][3] = offset;
			chunkInfo[i / 4][2] = sectorCount;
			if (i / 4 == 0)
			{
				//std::cout << std::hex << offset << "THIS IS THE OFFSET" << std::endl;
			}
			//std::cout << static_cast<int>(sectorCount) << " - SectorCount" << std::endl;
			//std::cout << "Read to " << std::hex << i << std::dec << std::endl;
		}
		std::cout << std::dec << chunkInfo[0][3] << std::endl << "test123" << std::endl;
	}

	// Writes a single chunk's compressed data into a vector. Also sets vector's size.
	void readChunk(int offset, int sectorCount, std::vector<unsigned char>& chunkDataCompressed, std::ifstream& inFile)
	{
		//Testing getting and setting of position in file stream
		/*std::cout << inFile.tellg() << " - Current Position in File, peek = " << inFile.peek() << "\n";
		int temp1 = inFile.tellg();
		inFile.seekg(0);
		std::cout << inFile.tellg() << " - Current Position in File, peek = " << inFile.peek() << "\n";
		inFile.seekg(temp1);
		std::cout << inFile.tellg() << " - Current Position in File, peek = " << inFile.peek() << "\n";*/
		// REMINDER - POSITION SHOULD BE SET BASED ON THE OFFSET, FOR TESTING PURPOSES LEFT AS IS

		// Move to starting position of payload based on offset
		inFile.seekg(offset * 4096);

		int length = 0;
		int temp = 0;
		char compressionType = 0;

		temp = inFile.get();
		temp = temp << 24;
		length = temp;

		temp = inFile.get();
		temp = temp << 16;
		length = length ^ temp;

		temp = inFile.get();
		temp = temp << 8;
		length = length ^ temp;

		temp = inFile.get();
		length = length ^ temp;

		compressionType = inFile.get();

		std::cout << "Length of compressed chunk data is " << length << /*" OR " << sectorCount <<*/ " bytes.\n" <<
			"Compression type of chunk data is " << int(compressionType) << ".\n";

		int payloadLen = length - 1;

		chunkDataCompressed.resize(payloadLen);

		for (int remaining = length - 1; remaining > 0; remaining--)
		{
			unsigned char tempChar = inFile.get();
			//chunkDataCompressed[4929 - remaining] = tempChar; // This 4929 looks weird, may need to be changed
			chunkDataCompressed[payloadLen - remaining] = tempChar;
		}
		//std::cout << inFile.tellg() << " - Current Position in File, peek = " << inFile.peek() << "\n";

	}

	//Real public:
public:
	Region() {
		for (int i = 0; i < 1024; i++)
			chunkInfo[i] = new int[4];
		chunkDataCompressed.resize(1024);
	}

	Region(const std::string& file) {
		for (int i = 0; i < 1024; i++)
			chunkInfo[i] = new int[4];
		chunkDataCompressed.resize(1024);

		filePath = file;
		//CopyHeaderData(header, filePath, iFile);
		/*for (int i = 0; i < 8192; i++)
			std::cout << std::hex << int(header[i]) << " ";*/
	}

	void setFilePath(const std::string& file) {
		filePath = file;
		//CopyHeaderData(header, filePath, iFile);
	}

	void startTask() {
		if (filePath == "") {
			std::cout << "\nError: No file path available.\n";
			return;
		}

		// Header section
		CopyHeaderData(header, filePath, iFile);
		ParseHeader(header, chunkInfo);

		// Chunks section
		for (int i = 0; i < 1024; i++) {
			readChunk(chunkInfo[i][3], chunkInfo[i][2], chunkDataCompressed[i], iFile);
			/*oFile.open("output/o" + std::to_string(i) + filePath, std::ios::binary);
			for (int j = 0; j < chunkDataCompressed[i].size(); j++) {
				oFile << chunkDataCompressed[i][j];
			}
			oFile.close();
			std::cout << "Last element in vector is " << std::hex << int(chunkDataCompressed[i].back()) << std::dec << "\n";*/
		}

		// Decompression section


		//std::cout << "Last element in vector is " << std::hex << int(chunkDataCompressed[0].back()) << "\n";
		for (int i = 0; i < 6; i++) {
			std::cout << iFile.get() << " ";
		}
	}

	~Region() {
		delete[] header;

		for (int i = 1; i < 1024; i++)
		{
			delete[] chunkInfo[i];
		}
		delete[] chunkInfo;
	}
};

#endif