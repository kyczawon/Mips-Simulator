#include <vector>
<<<<<<< ale
#include <cstdint>
#include <iostream>
=======
#include <stdint.h>
#include <iostream>
#include <cmath>
>>>>>>> master
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


<<<<<<< ale
void execute(vector <uint32_t> instructions, uint8_t* data, int32_t (&registers)[32], uint32_t& pc) {
	uint32_t instr = instructions[pc];
=======
void execute(uint32_t instr, uint8_t* data, int32_t (&registers)[32] , uint32_t& pc) {
>>>>>>> master
	//decode instruction and call correct subfunction
	//isolate opcode
	uint8_t opcode = instr >> 26;
	//check if R type
	if (opcode == 0) {
		//execute
		execute_R(instr, data, registers, pc);
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

void execute_R(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint32_t& pc, vector <uint32_t> &instructions) {
	//decode relevant operation from function code(LS 5 bits)
	uint32_t funct_code = (instr << 26) >> 26;
	uint32_t dest_reg, op1, op2, shift_amt;
	decode_fields_R(op1, op2, dest_reg, shift_amt, funct_code, instr);
		//filter funct 0x0_
	if (funct_code < 0x10){
		switch(funct_code) {
<<<<<<< ale
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
			case 8: jr(src_reg, registers, pc, instructions);
			//jalr 	rd, rs 		001001
			// 	case 9: jalr(---);
			//syscall 			001100
			// 	case 12: syscall(---);
			//break 			001101
			// 	case 13: brk(---);
=======
			// case 0: 		// sll 	rd, rt, sa 	000000
			// 	sll(---);
			// 	break;
			// case 1:		// srl 	rd, rt, sa 	000010
			// 	srl(---);
			// 	break;
			// case 3:		// sra 	rd, rt, sa 	000011
			// 	sra(---);
			// 	break;
			// case 4:		// sllv rd, rt, rs 	000100
			// 	sllv(---);
			// 	break;
			// case 6:		// srlv rd, rt, rs 	000110
			// 	srlv(---);
			// 	break;
			// case 7:		// srav rd, rt, rs 	000111
			// 	srav(---);
			// 	break;
			// case 8:		// jr 	rs 			001000
			// 	jr(op1, registers, pc);
			// 	break;
			// case 9:		// jalr rd, rs 		001001
			// 	jalr(---);
			// 	break;
			// case 12:		// syscall 			001100
			// 	syscall(---);
			// 	break;
			// case 13:		// break 			001101
			// 	brk(---);
			// 	break;
>>>>>>> master
		}
	}

		//filter funct 0x1_
	else if (funct_code < 0x20){
		switch(funct_code) {
			// case 16:	//mfhi 	rd 				010000
			// 	mfhi(---);
			// 	break;
			// case 17: 	//mthi 	rs 			010001
			// 	mthi(---);
			// 	break;
			// case 18: 	//mflo 	rd 			010010
			// 	mflo(---);
			// 	break;
			// case 19:	//mtlo 	rs 				010011
			// 	mtlo(---);
			// 	break;
			// case 24:	//mult 	rs, rt 			011000
			// 	mult(---);
			// 	break;
			// case 25: 	//multu rs, rt 		011001
			// 	multu(---);
			// 	break;
			// case 26: 	//div 	rs, rt 		011010
			// 	div(---);
			// 	break;
			// case 27: 	//divu 	rs, rt 		011011
			// 	divu(---);
			// 	break;
		}
	}

		//filter funct 0x2_
	else{
		switch(funct_code) {
			case 32:	//add 	rd, rs, rt 	100000
				add(dest_reg, op1, op2, registers);
				break;
			// case 33:	//addu 	rd, rs, rt 	100001
			// 	addu(---);
			// 	break;
			// case 34:	//sub 	rd, rs, rt 	100010
			// 	sub(---);
			// 	break;
			// case 35:	//subu 	rd, rs, rt 	100011
			// 	subu(---);
			// 	break;
			// case 36:	//and 	rd, rs, rt 	100100
			// 	AND(---);
			// 	break;
			// case 37:	//or 	rd, rs, rt 	100101
			// 	OR(---);
			// 	break;
			// case 38: //xor 	rd, rs, rt 	100110
			// 	XOR(---);
			// 	break;
			// case 39:	//nor 	rd, rs, rt 	100111
			// 	NOR(---);
			// 	break;
			// case 42: //slt 	rd, rs, rt 	101010
			// 	slt(---);
			// 	break;
			// case 43: //sltu 	rd, rs, rt 	101011
			// 	sltu(---);
			// 	break;
		}
	}
}

void execute_I (uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t &opcode){
	int32_t dest_reg, src_reg, immediate;
	decode_fields_I(dest_reg, src_reg, immediate, instr);


	//filter 0x0_
	if (opcode < 0x10){
		switch(opcode){
<<<<<<< ale
			//CORNER CASE --> DOUBLE POSSIBILITY
			//case 1: {
				//bgez 	rs, label 		000001
				//if (dest_reg == 1) bgez (---);
				//bltz 	rs, label 		000001 		
				//else bltz(---);
			//}

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
=======
			// case 1:		// CORNER CASE --> DOUBLE POSSIBILITY
			// 	// bgez 	rs, label 	000001
			// 	if (dest_reg == 1) bgez (---);
			// 	// bltz 	rs, label 	000001 		
			// 	else bltz(---);
			// 	break;
			// case 4:		// beq	rs, rt, label 		000100 
			// 	beq(---);
			// 	break;
			// case 5:		// bne 	rs, rt, label 		000101
			// 	bne(---);
			// 	break;
			// case 6:		// blez 	rs, label 		000110 
			// 	blez(---);
			// 	break;
			// case 7:		// bgtz 	rs, label 		000111
			// 	bgtz(---);
			// 	break;
			case 8:		// addi 	rt, rs, imm 		001000 
				addi(dest_reg, src_reg, immediate, registers);
				break;
			case 9:		// addiu 	rt, rs, imm 		001001
				addiu(dest_reg, src_reg, immediate, registers); 
				break;
			// case 10:	// slti 	rt, rs, imm 		001010 
			// 	slti(---);
			// 	break;
			// case 11:	// sltiu 	rt, rs, imm 		001011
			// 	sltiu(---);
			// 	break;
			// case 12:	// andi 	rt, rs, imm 		001100
			// 	andi(---); 
			// 	break;
			// case 13:	// ori 	rt, rs, imm 			001101
			// 	ori(---);
			// 	break;
			// case 14:	// xori 	rt, rs, imm 		001110 
			// 	xori(---);
			// 	break;
			case 15:	// lui 		rt, imm 			001111
				lui(dest_reg, immediate, registers); 
				break;
>>>>>>> master
		}
	}

	//filter 0x2_
	else if (opcode < 0x30){
		switch(opcode){
			// case 32:	//lb 	rt, imm(rs) 	100000
			// 	lb(---);
			// 	break;
			// case 33:	//lh 	rt, imm(rs) 	100001
			// 	lh(---);
			// 	break;
			// case 35:	//lw 	rt, imm(rs) 	100011
			// 	lw(---);
			// 	break;
			// case 36:	//lbu 	rt, imm(rs) 	100100
			// 	lbu(---);
			// 	break;
			// case 37:	//lhu 	rt, imm(rs) 	100101 
			// 	lhu(---);
			// 	break;
			case 40:	//sb 	rt, imm(rs) 	101000
				sb(registers[src_reg] + immediate, data, registers[dest_reg]);
				break;
			case 41:	//sh 	rt, imm(rs) 	101001
				sh(registers[src_reg] + immediate, data, registers[dest_reg]);
				break;
			case 43:	//sw 	rt, imm(rs) 	101011
				sw(registers[src_reg] + immediate, data, registers[dest_reg]);
				break;
		}
	}

	//filter 0x3_
	else{
		switch(opcode){
			// case 49:	//lwc1 	rt, imm(rs) 	110001 
			// 	lwc1(---);
			// 	break;
			// case 57:	//swc1	rt, imm(rs) 	111001 
			// 	swc1(---);
			// 	break;
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

void decode_fields_R (uint32_t &op1, uint32_t &op2, uint32_t &dest_reg, uint32_t &shift_amt, uint32_t &funct_code, const uint32_t &instr) {
	op1 = (instr << 6) >> 27;
	op2 = (instr << 11) >> 27;
	dest_reg = (instr << 16) >> 27;
	shift_amt = (instr << 21) >> 27;
	funct_code = (instr << 26) >> 26;

}


/////R TYPE INATRUCTIONS//////

void jr(uint32_t src_reg, int32_t (&registers)[32], uint32_t& pc, vector <uint32_t> instructions, uint8_t (&data)[DATA_SIZE]){
	execute()
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
	cout << "executing addiu" << endl;
}

void lui(int32_t &dest_reg, int32_t &immediate, int32_t (&registers)[32]){
	cout << "executing lui" << endl;
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
		cout << unsigned(value) << endl;
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
	cout << "executing sw" << endl;
	if (address % 4 != 0) exit(-11);
	int32_t lower_half, higher_half;
	lower_half = value & 0x0000FFFF;
	sh(address, data, lower_half);
	higher_half = (value & 0xFFFF0000) >> 16;
	sh(address + 2, data, higher_half);
}