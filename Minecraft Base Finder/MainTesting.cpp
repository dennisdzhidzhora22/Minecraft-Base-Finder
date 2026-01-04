#include <iostream>
#include <fstream>
#include <vector>
#include <string>
//#include <sstream>
//#include <cassert>
#include <zlib.h>
#include "Region.h"
//#include <>
//#define CHUNK 16384

using namespace std;

int main() {
	BlockFilter* defaultFilter = new BlockFilter;
	defaultFilter->loadDefault();

	Region* reg1 = new Region("r.0.0.mca", *defaultFilter);
	reg1->startTask();

	return 0;
}