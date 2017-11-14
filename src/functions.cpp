#include <vector>
#include <stdint.h>
#include <iostream>
#include <cmath>
#include "constants.hpp"
#include "functions.hpp"
using namespace std;

uint32_t bin_string_to_int(string input) {
	uint32_t x = 0;
	for (uint32_t i = 0; i < 32; i++) {
		if (input[i] == '1'){
			x += pow (2, 31-i);
		}
	}
	return x;
}

uint32_t read_byte(uint32_t address, uint8_t* data){
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

int32_t read_mem_s(uint32_t address, uint8_t* data){
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


void execute(uint32_t instr, uint8_t* data, int32_t (&registers)[32] , uint32_t& pc) {
	//decode instruction and call correct subfunction
	//isolate opcode
	uint8_t opcode = instr >> 26;
	//check if R type
	if (opcode == 0)
		//execute
		execute_R(instr, data, registers, pc);
	//check if J type
	else if (opcode == 2 || opcode == 3){
		//execute
		execute_J(instr, data, registers, opcode, pc);
	}
	//further decode and (eventually) execute
	else execute_I(instr, data, registers, opcode);
}

void execute_J(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t& opcode, uint32_t& pc) {

}

void execute_R(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint32_t& pc) {
	//decode relevant operation from function code(LS 5 bits)
	uint32_t funct_code = (instr << 26) >> 26;
	uint32_t dest_reg, op1, op2, shift_amt;
	decode_fields_R(dest_reg, op1, op2, shift_amt, funct_code, instr);
		//filter funct 0x0_
	if (funct_code < 0x10){
		switch(funct_code) {
			//sll 	rd, rt, sa 	000000
			// 	case 0: sll(---);
			//srl 	rd, rt, sa 	000010
			// 	case 1: srl(---);
			//sra 	rd, rt, sa 	000011
			// 	case 3: sra(---);
			//sllv 	rd, rt, rs 	000100
			// 	case 4: sllv(---);
			//srlv 	rd, rt, rs 	000110
			// 	case 6: srlv(---);
			//srav 	rd, rt, rs 	000111
			// 	case 7: srav(---);
			//jr 	rs 			001000
			//case 8: jr(src_reg, registers, pc);
			//jalr 	rd, rs 		001001
			// 	case 9: jalr(---);
			//syscall 			001100
			// 	case 12: syscall(---);
			//break 			001101
			// 	case 13: brk(---);
		}
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

void execute_I (uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t &opcode){
	int32_t dest_reg, src_reg, immediate;
	decode_fields_I(dest_reg, src_reg, immediate, instr);


	//filter 0x0_
	if (opcode < 0x10){
		switch(opcode){
			//CORNER CASE --> DOUBLE POSSIBILITY
			//case 1: {
				//bgez 	rs, label 		000001
				//if (dest_reg == 1) bgez (---);
				//bltz 	rs, label 		000001 		
				//else bltz(---);
			// }

			//beq 	rs, rt, label 	000100 
			//case 4: beq(---);
			//bne 	rs, rt, label 	000101
			//case 5: bne(---);
			//blez 	rs, label 		000110 
			//case 6: blez(---);
			//bgtz 	rs, label 		000111
			//case 7: bgtz(---); 
			//addi 	rt, rs, imm 	001000 
			case 8: addi(dest_reg, src_reg, immediate, registers);
			//addiu rt, rs, imm 	001001
			case 9: addiu(dest_reg, src_reg, immediate, registers); 
			//slti 	rt, rs, imm 	001010 
			//case 10: slti(---);
			//sltiu rt, rs, imm 	001011
			//case 11: sltiu(---); 
			//andi 	rt, rs, imm 	001100
			//case 12: andi(---); 
			//ori 	rt, rs, imm 	001101
			//case 13: ori(---); 
			//xori 	rt, rs, imm 	001110 
			//case 14: xori(---);
			//lui 	rt, imm 		001111
			case 15: lui(dest_reg, immediate, registers); 
		}
	}

	//filter 0x1_
	else if (opcode < 0x20){
		switch(opcode){
			// //lb 	rt, imm(rs) 	100000
			// case 32: lb(---); 
			// //lh 	rt, imm(rs) 	100001
			// case 33: lh(---); 
			// //lw 	rt, imm(rs) 	100011
			// case 35: lw(---); 
			// //lbu 	rt, imm(rs) 	100100
			// case 36: lbu(---); 
			// //lhu 	rt, imm(rs) 	100101 
			// case 37: lhu(---);
			// //sb 	rt, imm(rs) 	101000
			case 40: sb(registers[src_reg] + immediate, data, registers[dest_reg]);
			// //sh 	rt, imm(rs) 	101001
			case 41: sh(registers[src_reg] + immediate, data, registers[dest_reg]); 
			// //sw 	rt, imm(rs) 	101011
			case 43: sw(registers[src_reg] + immediate, data, registers[dest_reg]);
		}
	}

	//filter 0x2_
	else{
		switch(opcode){
			// //lwc1 	rt, imm(rs) 	110001 
			// case 49: lwc1(---);
			// //swc1	rt, imm(rs) 	111001 
			// case 57: swc1(---);
		}
	}	

}

void decode_fields_I (int32_t &dest_reg, int32_t &src_reg, int32_t& immediate, const int32_t &instruction){
	dest_reg = (instruction << 11) >> 27;
	src_reg = (instruction << 6) >> 27;
	//for the immediate(16 bits long) sign extension is necessary
	immediate = instruction << 16;
	if (immediate >= 0) immediate = immediate >> 16;
	else immediate = (immediate >> 16) | 0xFFFF0000;
}

void decode_fields_R (uint32_t &dest_reg, uint32_t &op1, uint32_t &op2, uint32_t &shift_amt, uint32_t &funct_code, const uint32_t &instr) {
	dest_reg = (instr << 16) >> 27;
	op1 = (instr << 6) >> 27;
	op2 = (instr << 11) >> 27;
	shift_amt = (instr << 21) >> 27;
	funct_code = (instr << 26) >> 26;

}


/////R TYPE INATRUCTIONS//////

void jr(uint32_t src_reg, int32_t (&registers)[32], uint32_t& pc){
	pc = (registers[src_reg] / 4) - 1;
}


void add(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	int32_t source1 = registers[op1];
	int32_t source2 = registers[op2];
	int64_t sum = source1 + source2;
	//check for signed/unsigned overflow
	int32_t sum2 = source1 + source2;
	if (sum != sum2) exit(-10);
	else registers[dest_reg] =  sum2;
}

void sll(uint32_t dest_reg, uint32_t op, uint32_t shift_amt, int32_t (&registers)[32]){
	int64_t result = registers[op] << shift_amt;
	int32_t result2 = registers[op] << shift_amt;
	//check overflow
	if (result != result2) exit (-10);
	else registers[dest_reg] = result2;
}



//////I TYPE INSTRUCTIONS///////

void addi(int32_t &dest_reg, int32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]){
	int32_t source = registers[src_reg];
	int64_t sum = source + immediate;
	int32_t sum2 = source + immediate;
	if (sum != sum2) exit(-10);
	else registers[dest_reg] = sum2;
}

void addiu(int32_t &dest_reg, int32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]){
	registers[dest_reg] = registers[src_reg] + immediate;
}

void lui(int32_t &dest_reg, int32_t &immediate, int32_t (&registers)[32]){
	registers[dest_reg] = immediate << 16;
}

void sb(uint32_t address, uint8_t* data, uint8_t value){
		//check if mem to be accessed is between correct bounds for data space
	if (address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE){
		//remove data offset and write
		data[address - ADDR_DATA] = value;
	}
	//else check if instruction is trying to write ADDR_PUTC location
	else if (address > 0x30000003 && address < 0x30000008){
		int8_t temp = (int8_t) value;
		cout << temp << endl;
	}
	//otherwise return error code
	else exit(-11);
}

void sh(uint32_t address, uint8_t* data, int32_t value){
	if (address % 2 != 0) exit(-11);
	uint8_t lower_byte, higher_byte;
	lower_byte = value & 0x000000FF;
	sb(address, data, lower_byte);
	higher_byte = (value & 0x0000FF00) >> 8;
	sb(address++, data, higher_byte);
}

void sw(uint32_t address, uint8_t* data, int32_t value){
	if (address % 4 != 0) exit(-11);
	int32_t lower_half, higher_half;
	lower_half = value & 0x0000FFFF;
	sh(address, data, lower_half);
	higher_half = (value & 0xFFFF0000) >> 16;
	sh(address + 2, data, higher_half);
}