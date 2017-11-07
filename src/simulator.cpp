#include <cstdlib>
#include <fstream>
#include <vector>
//#include <cstdint>
#include "constants.hpp"
// #include "functions.hpp"
using namespace std;

int main(int argc, char* argv[])
{
	//initialize memory space
		//instruction memory as vector
	vector<unsigned char> instructions;
		//registers file as array
	int registers[32] = {0};
		//data space as array
	unsigned char data[DATA_SIZE] = {0}; 


	//global variables
	unsigned int pc = 0;


	//open Binary file
	ifstream infile;
	infile.open(argv[0]);

	//if IO error exit with correct exit code
	if (!infile.is_open()) {
		exit(-21);
	}

	//load instructions from Binary file into correct location in RAM 
	//each instruction is loaded into 4 separate bytes
	unsigned int x;
	int offset = ADDR_INSTR;
	while (infile >> x) {
		for (int i = 0; i <=24; i+=8) {
			instructions.push_back(x >> i);
		}
	}

	//execute instructions
	while (pc < instructions.size()){
	//execute and check if ret_value is ok
		if (execute(instructions, data, registers, pc)) {
			//deal with exit code
		}

	}

  exit(-10);
}

