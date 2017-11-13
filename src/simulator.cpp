#include <cstdlib>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include "constants.hpp"
#include "functions.hpp"

int32_t main(int32_t argc, int8_t* argv[])
{
	//initialize memory space
		//instruction memory as vector
	vector<uint32_t> instructions;
		//registers file as array
	int32_t registers[32] = {0};
		//data space as array
	uint8_t* data = new uint8_t[DATA_SIZE];

	 for (uint32_t i = 0; i < DATA_SIZE; i++) {
		data[i] = 0;
	 }

	//global variables
	uint32_t pc = 0;

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
	int32_t offset = ADDR_INSTR;
	while (infile >> input) {
		uint32_t x = bin_string_to_int(input);
		instructions.push_back(x);
	}

	//execute instructions
	while (pc < instructions.size()){
		uint32_t instr = instructions[pc];
		execute(instr, data, registers, pc);
		pc+=1;
	}

  exit(-10);
}

uint32_t bin_string_to_int(string input) {
	uint32_t x = 0;
	for (uint32_t i = 0; i < 32; i++) {
		if (input[i] == '1'){
			x += pow (2, 31-i);
		}
	}
	return x;
}

uint32_t read_byte(uint32_t address, uint8_t (&data)[DATA_SIZE]){
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

int32_t read_mem_s(uint32_t address, uint8_t (&mem)){
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

int32_t read_reg(uint32_t address, int32_t (&registers)[32]){
	return 0;
}

void execute(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint32_t& pc) {
	cout << "enter execute" << endl;
	//decode instruction and call correct subfunction
	//isolate opcode
	uint8_t opcode = instr >> 26;
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

void execute_J(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t& opcode, uint32_t& pc) {
}
void execute_I(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t& opcode) {

}

void execute_R(uint32_t instr, uint8_t* data, int32_t (&registers)[32]) {
	//decode relevant operation from function code(LS 5 bits)
	uint32_t funct_code = (instr << 26) >> 26;
	uint32_t dest_reg, op1, op2, shift_amt;
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

void decode_fields_R (uint32_t &dest_reg, uint32_t &op1, uint32_t &op2, uint32_t &shift_amt, uint32_t &funct_code, const uint32_t &instr){
	dest_reg = (instr << 16) >> 27;
	op1 = (instr << 6) >> 27;
	op2 = (instr << 11) >> 27;
	shift_amt = (instr << 21) >> 27;
	funct_code = (instr << 26) >> 26;

}

void add(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	cout << "add" << endl;
	int32_t source1 = registers[op1];
	int32_t source2 = registers[op2];
	long sum = source1 + source2;
	//check for signed/unsigned overflow
	int32_t sum2 = source1 + source2;
	if (sum != sum2) exit(-10);
	else registers[dest_reg] =  sum2;
}
