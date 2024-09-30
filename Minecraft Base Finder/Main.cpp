#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
using namespace std;

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

void ParseHeader(unsigned char header2[], int** chunkData)
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
		chunkData[i/4][3] = offset;
		chunkData[i/4][2] = sectorCount;
		if (i / 4 == 0)
		{
			cout << hex << offset << "THIS IS THE OFFSET" << endl;
		}
		cout << static_cast<int>(sectorCount) << " - SectorCount" << endl;
		cout << "Read to " << hex << i << dec << endl;
	}
	cout << dec << chunkData[0][3] << endl << "test123" << endl;
}

void readChunk(int offset, int sectorCount, ifstream& inFile)
{
	int length = 0;
	int temp = 0;
	char compressionType = 0;

	temp = inFile.get();
	temp = temp << 24;
	offset = temp;

	temp = inFile.get();
	temp = temp << 16;
	offset = offset ^ temp;

	temp = inFile.get();
	temp = temp << 8;
	offset = offset ^ temp;

	temp = inFile.get();
	offset = offset ^ temp;

	//Maybe make a readBytes() function which takes in number of bytes to read and variable/data structure to write them to.

	compressionType = inFile.get();
}

int main() {
	unsigned char* header = new unsigned char[8192]();

	int** chunkData = new int* [1024];

	for (int i = 0; i < 1024; i++)
	{
		assert(i < 1024);
		chunkData[i] = new int[4];
	}

	//cout << "\"" << static_cast<int>(header[3]) << "\"" << endl;

	ifstream iFile;
	//ofstream oFile;

	/*iFile.open("r.0.0.mca");
	iFile.read(header, 8192);*/

	CopyHeaderData(header, iFile);
	//cout << static_cast<int>(header[2]) << endl;
	ParseHeader(header, chunkData);

	//cout << "\"" << static_cast<int>(header[5]) << "\"" << endl;



	iFile.close();
	cout << "Finished";

	delete[] header;

	for (int i = 1; i < 1024; i++)
	{
		delete[] chunkData[i];
	}
	
	//delete[] chunkData[0];   For some reason, this doesn't work.
	//NEVERMIND - I was writing to index 4, which is out of bounds.

	delete[] chunkData;

	return 0;
}