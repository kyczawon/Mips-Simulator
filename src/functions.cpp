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

void execute(vector <unsigned char> &instructions, unsigned char (&data)[DATA_SIZE], int (&registers)[32] , int& pc) {

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


void execute_R(---){
	//decode relevant operation from function code(LS 5 bits)
	int funct_code = (instr << 26) >> 26;
		//filter funct 0x0_
	if (funct_code < 0x10){
		//sll 	rd, rt, sa 	000000
		if (funct_code == 0) sll(---);
		//srl 	rd, rt, sa 	000010
		else if (funct_code == 2) srl(---);
		//sra 	rd, rt, sa 	000011
		else if (funct_code == 3) return sra(---);
		//sllv 	rd, rt, rs 	000100
		else if (funct_code == 4) return sllv(---);
		//srlv 	rd, rt, rs 	000110
		else if (funct_code == 6) return srlv(---);
		//srav 	rd, rt, rs 	000111
		else if (funct_code == 7) return srav(---);
		//jr 	rs 			001000
		else if (funct_code == 8) return jr(---);
		//jalr 	rd, rs 		001001
		else if (funct_code == 9) return jalr(---);
		//syscall 			001100
		else if (funct_code == 12) return syscall(---);
		//break 			001101
		else if (funct_code == 13) return brk(---);


	}

		//filter funct 0x1_
	else if (funct_code < 0x20){
		//mfhi 	rd 			010000
		if (funct_code == 16) return mfhi(---);
		//mthi 	rs 			010001
		else if (funct_code == 17) return mthi(---);
		//mflo 	rd 			010010
		else if (funct_code == 18) return mflo(---);
		//mtlo 	rs 			010011
		else if (funct_code == 19) return mtlo(---);
		//mult 	rs, rt 		011000
		else if (funct_code == 24) return mult(---);
		//multu rs, rt 		011001
		else if (funct_code == 25) return multu(---);
		//div 	rs, rt 		011010
		else if (funct_code == 26) return div(---);
		//divu 	rs, rt 		011011
		else if (funct_code == 27) return divu(---);

	}

		//filter funct 0x2_
	else{
		//add 	rd, rs, rt 	100000
		if (funct_code == 32) return add(---);
		//addu 	rd, rs, rt 	100001
		else if (funct_code == 33) return addu(---);
		//sub 	rd, rs, rt 	100010
		else if (funct_code == 34) return sub(---);
		//subu 	rd, rs, rt 	100011
		else if (funct_code == 35) return subu(---);
		//and 	rd, rs, rt 	100100
		else if (funct_code == 36) return AND(---);
		//or 	rd, rs, rt 	100101
		else if (funct_code == 37) return OR(---);
		//xor 	rd, rs, rt 	100110
		else if (funct_code == 38) return XOR(---);
		//nor 	rd, rs, rt 	100111
		else if (funct_code == 39) return NOR(---);
		//slt 	rd, rs, rt 	101010
		else if (funct_code == 42) return slt(---);
		//sltu 	rd, rs, rt 	101011
		else if (funct_code == 43) return sltu(---);

	}

	//if funct_code not acceptable return error
	return (---);

}

void decode_fields_R (int &dest_reg, int &op1, int &op2, int &shift_amt, int &funct_code, const int &instruction){
	dest_reg = (instruction << 16) >> 27;
	op1 = (instruction << 6) >> 27;
	op2 = (instruction << 11) >> 27;
	shift_amt = (instruction << 21) >> 27;
	funct_code = (instr << 26) >> 26;

}

void add(int dest_reg, op1, op2, int (&registers)[32]){
	int source1 = registers[op1];
	int source2 = registers[op2];
	long sum = source1 + source2;
	//check for signed/unsigned overflow
	int sum2 = source1 + source2;
	if (sum != sum2) exit(-10);
	else registers[dest_reg] =  sum2;
}

