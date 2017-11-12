#include <vector>
#include "constants.hpp"
using namespace std;

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
		}
	}
	//otherwise return error code
	else exit(-11);
}


int read_reg(unsigned int address, int (&registers)[32]){
	
}

void execute(vector <unsigned char> &instructions, unsigned char* data, int (&registers)[32], unsigned int& pc) {

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

void execute_J(unsigned int instr, unsigned char* data, int (&registers)[32], unsigned char& opcode, unsigned int& pc);
void execute_I(unsigned int instr, unsigned char* data, int (&registers)[32], unsigned char& opcode);

void execute_R(unsigned int instr, unsigned char* data, int (&registers)[32]) {
	//decode relevant operation from function code(LS 5 bits)
	int funct_code = (instr << 26) >> 26;
	int dest_reg, op1, op2, shift_amt;
	decode_fields_R (dest_reg, op1, op2, shift_amt, funct_code, instr);
		//filter funct 0x0_
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

void decode_fields_R (int &dest_reg, int &op1, int &op2, int &shift_amt, int &funct_code, const int &instr){
	dest_reg = (instr << 16) >> 27;
	op1 = (instr << 6) >> 27;
	op2 = (instr << 11) >> 27;
	shift_amt = (instr << 21) >> 27;
	funct_code = (instr << 26) >> 26;

}

void add(int dest_reg, int op1, int op2, int (&registers)[32]){
	int source1 = registers[op1];
	int source2 = registers[op2];
	long sum = source1 + source2;
	//check for signed/unsigned overflow
	int sum2 = source1 + source2;
	if (sum != sum2) exit(-10);
	else registers[dest_reg] =  sum2;
}