#include <cstdlib>
#include <fstream>
#include "constants.hpp"
// #include "functions.hpp"
using namespace std;

int main(int argc, char* argv[])
{
	//initialize RAM and Register File
	unsigned char mem[RAM_SIZE] = {0};
	int reg[32] = {0};

	//global variables
	unsigned int pc = ADDR_INSTR;


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
			mem[offset++] = x >> i;
		}
	}

	//execute instructions
	while (mem[pc] != 0){
		//check if ret_value is ok
		// if (execute(mem, pc)) {
		// 	//deal with exit code
		// }
	}

  exit(-10);
}

