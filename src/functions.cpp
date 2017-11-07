#include <vector>
#include "constants.hpp"
using namespace std;
int execute(vector <unsigned char> &instructions, unsigned char (&data)[DATA_SIZE], unsigned char (&registers)[32] , int& pc) {

	//get instruction from 4 bytes in mem
	unsigned int instr = instructions[pc];
	for (int x = 1; x < 4; x++){
		instr = instr << 8;
		instr += instructions[pc + x];
	}

	//decode instruction and call correct subfunction
	//isolate opcode
	unsigned char opcode = instr >> 26;
	//check if R type
	if (opcode == 0)
		//execute
		execute_R(instr, data, registers);
	//check if J type
	else if (opcode == 2 || opcode == 3){
		//execute
		execute_J(instr, data, registers, opcode, pc);
	}
	//further decode and (eventually) execute
	else execute_I(instr, data, registers, opcode);
}




