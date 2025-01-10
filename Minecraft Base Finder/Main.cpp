#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include "zlib/zlib.h"
//#include <>
#define CHUNK 16384

using namespace std;

// Doubles the size of a dynamic array
template <typename T>
void resizeDynArr(T* &arr, int &size)
{
	int newSize = size * 2;
	T* newArray = new T[newSize];

	for (int i = 0; i < newSize; i++)
		newArray[i] = 0;
	for (int i = 0; i < size; i++)
		newArray[i] = arr[i];

	size = newSize;
	delete[] arr;
	arr = newArray;
}

// Copies header data into array
void CopyHeaderData(unsigned char header1[], ifstream& inFile)
{
	inFile.open("r.0.0.mca", ios::binary);
	//inFile.read(header1, 8192);  read() doesn't work with unsigned char arrays

	int i = 0;

	if (inFile.is_open())
	{
		while (inFile.good() && i < 8192)
		{
			header1[i] = inFile.get();
			//inFile.get(reinterpret_cast<char*>(header1), 8192);
			i++;
		}
	}
	else
		cout << "Could not open file.\n";

	/*for (int i = 0; i < 8192; i++)
	{
		header1[i] << inFile.get();
	}*/

	//outFile.close();
}

// Reads and organizes data from header array into chunkInfo array
void ParseHeader(unsigned char header2[], int** chunkInfo)
{
	int offset = 0; //Offset from start of file in 4KiB sections.
	int temp = 0;
	unsigned char sectorCount = 0; //Number of 4KiB sectors used, rounded up.

	for (int i = 0; i < 4096; i+=4)
	{
		temp = header2[i];
		temp = temp << 16;
		offset = temp;
		cout << hex << temp << endl;
		cout << hex << offset << endl;

		temp = header2[i + 1];
		temp = temp << 8;
		offset = offset ^ temp;
		cout << hex << temp << endl;
		cout << hex << offset << endl;

		temp = header2[i + 2];
		//cout << dec << temp << "TEST" << endl;
		offset = offset ^ temp;
		cout << hex << temp << endl;
		cout << hex << offset << endl;

		sectorCount = header2[i + 3];

		assert(i < 4096);
		chunkInfo[i/4][3] = offset;
		chunkInfo[i/4][2] = sectorCount;
		if (i / 4 == 0)
		{
			cout << hex << offset << "THIS IS THE OFFSET" << endl;
		}
		cout << static_cast<int>(sectorCount) << " - SectorCount" << endl;
		cout << "Read to " << hex << i << dec << endl;
	}
	cout << dec << chunkInfo[0][3] << endl << "test123" << endl;
}

// Writes a single chunk's compressed data into a vector
void readChunk(int offset, int sectorCount, vector<unsigned char>& chunkDataCompressed, ifstream& inFile)
{
	//Testing getting and setting of position in file stream
	cout << inFile.tellg() << " - Current Position in File, peek = " << inFile.peek() << "\n";
	int temp1 = inFile.tellg();
	inFile.seekg(0);
	cout << inFile.tellg() << " - Current Position in File, peek = " << inFile.peek() << "\n";
	inFile.seekg(temp1);
	cout << inFile.tellg() << " - Current Position in File, peek = " << inFile.peek() << "\n";
	// REMINDER - POSITION SHOULD BE SET BASED ON THE OFFSET, FOR TESTING PURPOSES LEFT AS IS

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

	//Maybe make a readBytes() function which takes in number of bytes to read and variable/data structure to write them to.

	compressionType = inFile.get();

	cout << "Length of compressed chunk data is " << length << " OR " << sectorCount << " bytes.\n" <<
		"Compression type of chunk data is " << int(compressionType) << ".\n";

	chunkDataCompressed.resize(length - 1);

	for (int remaining = length - 1; remaining > 0; remaining--)
	{
		unsigned char tempChar = inFile.get();
		chunkDataCompressed[4929 - remaining] = tempChar; // This 4929 looks weird, may need to be changed
	}
	cout << inFile.tellg() << " - Current Position in File, peek = " << inFile.peek() << "\n";

}

int main() {
	unsigned char* header = new unsigned char[8192]();

	int** chunkInfo = new int* [1024]; // Array of arrays which hold offset and sector size for each chunk

	for (int i = 0; i < 1024; i++)
	{
		assert(i < 1024);
		chunkInfo[i] = new int[4]; // Currently you can hold 4 integers for each chunk, but this can be changed if needed
	}

	//cout << "\"" << static_cast<int>(header[3]) << "\"" << endl;

	ifstream iFile;
	//istringstream iString;
	ofstream oFile;

	/*iFile.open("r.0.0.mca");
	iFile.read(header, 8192);*/

	CopyHeaderData(header, iFile);
	//cout << static_cast<int>(header[2]) << endl;
	ParseHeader(header, chunkInfo);

	//cout << "\"" << static_cast<int>(header[5]) << "\"" << endl;

	vector<unsigned char> chunkDataComp; // Holds compressed data for 1 chunk

	readChunk(chunkInfo[0][3], chunkInfo[0][2], chunkDataComp, iFile);

	unsigned char* chunkDataComp1 = new unsigned char[chunkDataComp.size()];
	for (int i = 0; i < chunkDataComp.size(); i++)
	{
		chunkDataComp1[i] = chunkDataComp[i];
	}

	cout << "Last element in vector is " << hex << int(chunkDataComp.back()) << "\n";


	/*for (int i = 0; i < 4929; i++)
	{
		cout << int(chunkDataComp[i]) << "\n";
	}*/

	/*string str1(chunkDataComp.begin(), chunkDataComp.end());
	cout << str1 << "\n";

	istringstream strNew1(str1, ios::binary);

	string str2;
	ostringstream strNew2(str2, ios::binary);*/

	uLong destLen = chunkDataComp.size(); // uLong - unsigned long int

	//vector<unsigned char> chunkDataUncomp;

	//uLong destLenBound = compressBound(chunkDataComp.size());
	//uLong* destLenBoundPtr = &destLenBound;
	unsigned char* chunkDataUncomp = new unsigned char[destLen]();
	int len = destLen;

	int ret = uncompress(chunkDataUncomp, &destLen, chunkDataComp1, chunkDataComp.size());

	while (ret == Z_BUF_ERROR && ret != Z_OK) // While not enough space in destination, resize and try again
	{
		if (ret == Z_MEM_ERROR)
		{
			std::cout << "\nZ_MEM_ERROR\n";
			break;
		}
		resizeDynArr(chunkDataUncomp, len);
		destLen = len;
		ret = uncompress(chunkDataUncomp, &destLen, chunkDataComp1, chunkDataComp.size());
	}

	oFile.open("uncompressedOutput.txt", ios::binary);
	//adler32()

	for (int i = 0; i < len; i++) // Writes uncompressed chunk data to file
		oFile << chunkDataUncomp[i];


	//vector<unsigned char> unComp = inflate(chunkDataComp);


	iFile.close();
	oFile.close();
	cout << "Finished";

	delete[] header;
	delete[] chunkDataUncomp;

	for (int i = 1; i < 1024; i++)
	{
		delete[] chunkInfo[i];
	}
	
	//delete[] chunkData[0];   For some reason, this doesn't work.
	//NEVERMIND - I was writing to index 4, which is out of bounds.

	delete[] chunkInfo;

	return 0;
}