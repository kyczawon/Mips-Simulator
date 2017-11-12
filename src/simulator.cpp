#include <cstdlib>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include "constants.hpp"
#include "functions.hpp"

int main(int argc, char* argv[])
{
	//initialize memory space
		//instruction memory as vector
	vector<unsigned int> instructions;
		//registers file as array
	int registers[32] = {0};
		//data space as array
	unsigned char* data = new unsigned char[DATA_SIZE];

	 for (unsigned int i = 0; i < DATA_SIZE; i++) {
		data[i] = 0;
	 }

	//global variables
	unsigned int pc = 0;

	//open Binary file
	ifstream infile;
	infile.open(argv[1]);

	//if IO error exit with correct exit code
	if (!infile.is_open()) {
		exit(-21);
	}

	//load instructions from Binary file into correct location in RAM 
	//each instruction is loaded into 4 separate bytes
	string input;
	int offset = ADDR_INSTR;
	while (infile >> input) {
		unsigned int x = bin_string_to_int(input);
		instructions.push_back(x);
	}

	//execute instructions
	while (pc < instructions.size()){
		unsigned int instr = instructions[pc];
		execute(instr, data, registers, pc);
		pc+=1;
	}

  exit(-10);
}

unsigned int bin_string_to_int(string input) {
	unsigned int x = 0;
	for (unsigned int i = 0; i < 32; i++) {
		if (input[i] == '1'){
			x += pow (2, 31-i);
		}
	}
	return x;
}

unsigned int read_byte(unsigned int address, unsigned char (&data)[DATA_SIZE]){
	//check if mem to be accessed is between correct bounds for data space
	if (address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE){
		//remove data offset and read
		return data[address - ADDR_DATA];
	}
	//else check if instruction is trying to read ADDR_GETC location
	else if (address == 0x30000000){
		//read from keyboard
		//
		//---TO DO---
		//
		return 0;
	}
	//otherwise return error code
	else exit(-11);
}

int read_mem_s(unsigned int address, unsigned char (&mem)){
	//read only if address is valid
	if (address % 4 == 0){
		//check if mem to be accessed is between correct bounds for data space
		if (address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE){
			//remove data offset and read

		}
		//else check if instruction is trying to read ADDR_GETC location
		else if (address == 0x30000000){
			//read from keyboard
			//
			//---TO DO---
			//
			return 0;
		}
	}
	//otherwise return error code
	else exit(-11);
	return 0;
}

int read_reg(unsigned int address, int (&registers)[32]){
	return 0;
}

void execute(unsigned int instr, unsigned char* data, int (&registers)[32], unsigned int& pc) {
	cout << "enter execute" << endl;
	//decode instruction and call correct subfunction
	//isolate opcode
	unsigned char opcode = instr >> 26;
	//check if R type
	if (opcode == 0) {
		//execute
		cout << "enter R type" << endl;
		execute_R(instr, data, registers);
	//check if J type
	} else if (opcode == 2 || opcode == 3){
		//execute
		execute_J(instr, data, registers, opcode, pc);
	}
	//further decode and (eventually) execute
	else execute_I(instr, data, registers, opcode);
}

void execute_J(unsigned int instr, unsigned char* data, int (&registers)[32], unsigned char& opcode, unsigned int& pc) {
}
void execute_I(unsigned int instr, unsigned char* data, int (&registers)[32], unsigned char& opcode) {

}

void execute_R(unsigned int instr, unsigned char* data, int (&registers)[32]) {
	//decode relevant operation from function code(LS 5 bits)
	unsigned int funct_code = (instr << 26) >> 26;
	unsigned int dest_reg, op1, op2, shift_amt;
	decode_fields_R(dest_reg, op1, op2, shift_amt, funct_code, instr);
		//filter funct 0x0_
	cout << "dest_reg " << dest_reg << " op1 " << op1 << " op2 " << op2 << " shift " << shift_amt << " funct " << funct_code << " instr " << instr << endl;	
	if (funct_code < 0x10){
		// switch(funct_code) {
		// 	//sll 	rd, rt, sa 	000000
		// 	case 0: sll(---);
		// 	//srl 	rd, rt, sa 	000010
		// 	case 1: srl(---);
		// 	//sra 	rd, rt, sa 	000011
		// 	case 3: sra(---);
		// 	//sllv 	rd, rt, rs 	000100
		// 	case 4: sllv(---);
		// 	//srlv 	rd, rt, rs 	000110
		// 	case 6: srlv(---);
		// 	//srav 	rd, rt, rs 	000111
		// 	case 7: srav(---);
		// 	//jr 	rs 			001000
		// 	case 8: jr(---);
		// 	//jalr 	rd, rs 		001001
		// 	case 9: jalr(---);
		// 	//syscall 			001100
		// 	case 12: syscall(---);
		// 	//break 			001101
		// 	case 13: brk(---);
		// }
	}

		//filter funct 0x1_
	else if (funct_code < 0x20){
		// switch(funct_code) {
		// 	//mfhi 	rd 			010000
		// 	case 16: mfhi(---);
		// 	//mthi 	rs 			010001
		// 	case 17: mthi(---);
		// 	//mflo 	rd 			010010
		// 	case 18: mflo(---);
		// 	//mtlo 	rs 			010011
		// 	case 19: mtlo(---);
		// 	//mult 	rs, rt 		011000
		// 	case 24: mult(---);
		// 	//multu rs, rt 		011001
		// 	case 25: multu(---);
		// 	//div 	rs, rt 		011010
		// 	case 26: div(---);
		// 	//divu 	rs, rt 		011011
		// 	case 27: divu(---);
		// }
	}

		//filter funct 0x2_
	else{
		switch(funct_code) {
			cout << "decoded" << endl;
			//add 	rd, rs, rt 	100000
			case 32: add(dest_reg, op1, op2, registers);
			// //addu 	rd, rs, rt 	100001
			// case 33: addu(---);
			// //sub 	rd, rs, rt 	100010
			// case 34: sub(---);
			// //subu 	rd, rs, rt 	100011
			// case 35: subu(---);
			// //and 	rd, rs, rt 	100100
			// case 36: AND(---);
			// //or 	rd, rs, rt 	100101
			// case 37: OR(---);
			// //xor 	rd, rs, rt 	100110
			// case 38: XOR(---);
			// //nor 	rd, rs, rt 	100111
			// case 39: NOR(---);
			// //slt 	rd, rs, rt 	101010
			// case 42: slt(---);
			// //sltu 	rd, rs, rt 	101011
			// case 43: sltu(---);
		}
	}
}

void decode_fields_R (unsigned int &dest_reg, unsigned int &op1, unsigned int &op2, unsigned int &shift_amt, unsigned int &funct_code, const unsigned int &instr){
	dest_reg = (instr << 16) >> 27;
	op1 = (instr << 6) >> 27;
	op2 = (instr << 11) >> 27;
	shift_amt = (instr << 21) >> 27;
	funct_code = (instr << 26) >> 26;

}

void add(unsigned int dest_reg, unsigned int op1, unsigned int op2, int (&registers)[32]){
	cout << "add" << endl;
	int source1 = registers[op1];
	int source2 = registers[op2];
	long sum = source1 + source2;
	//check for signed/unsigned overflow
	int sum2 = source1 + source2;
	if (sum != sum2) exit(-10);
	else registers[dest_reg] =  sum2;
}
