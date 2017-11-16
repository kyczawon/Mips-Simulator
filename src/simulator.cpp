#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <stdint.h>
#include <cmath>
#include <iostream>
#include "constants.hpp"
#include "functions.hpp"

int main(int argc, char* argv[]){
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
	 //program counter
	uint32_t pc = 0;
	 //HI, LO special registers
	int32_t HiLo[2] = {0};

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
		uint32_t x = bin_string_to_uint32_t(input);
		instructions.push_back(x);
	}



	//execute instructions
	while (pc < instructions.size()){
		cout << "executing" << endl;
		execute(instructions, data, registers, pc, HiLo);
		pc+=1;
	}

  exit(-10);
}

uint32_t bin_string_to_uint32_t(string input) {
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

void execute(vector <uint32_t> &instructions, uint8_t* data, int32_t (&registers)[32], uint32_t& pc, int32_t (&HiLo)[2]) {
	cout << "enter execute" << endl;
	uint32_t instr = instructions[pc];
	//decode instruction and call correct subfunction
	//isolate opcode
	uint8_t opcode = instr >> 26;
	//check if R type
	if (opcode == 0)
		//execute
		execute_R(instr, data, registers, pc, instructions, HiLo);
	//check if J type
	else if (opcode == 2 || opcode == 3){
		//execute
		execute_J(instr, data, registers, opcode, pc, instructions, HiLo);
	}
	//further decode and (eventually) execute
	else execute_I(instr, data, registers, opcode);
}

void execute_J(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t& opcode, uint32_t& pc, vector<uint32_t> &instructions, int32_t (&HiLo)[2]) {
	//Jump (J)
	if (opcode == 2){ 
		j(instructions, data, registers, pc, HiLo);
	}
	//Jump and Link (JAL)
	else if (opcode == 3){
		jal(instructions, data, registers, pc, HiLo);
	}
	//invalid instruction
	else exit(-12);
}

void execute_R(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint32_t& pc, vector<uint32_t> &instructions, int32_t (&HiLo)[2]) {
	//decode relevant operation from function code(LS 5 bits)
	uint32_t funct_code = (instr << 26) >> 26;
	uint32_t dest_reg, op1, op2, shift_amt;
	decode_fields_R(op1, op2, dest_reg, shift_amt, funct_code, instr);
		//filter funct 0x0_
	if (funct_code < 0x10){
		switch(funct_code) {
			case 0: 		// sll 	rd, rt, sa 	000000
				sll(dest_reg, op2, shift_amt, registers);
				break;
			case 1:		// srl 	rd, rt, sa 	000010
			 	srl(dest_reg, op2, shift_amt, registers);
			 	break;
			case 3:		// sra 	rd, rt, sa 	000011
			 	sra(dest_reg, op2, shift_amt, registers);
			 	break;
			case 4:		// sllv rd, rt, rs 	000100
			 	sllv(dest_reg, op1, op2, registers);
			 	break;
			case 6:		// srlv rd, rt, rs 	000110
			 	srlv(dest_reg, op1, op2, registers);
			 	break;
			case 7:		// srav rd, rt, rs 	000111
			 	srav(dest_reg, op1, op2, registers);
			 	break;
			case 8:		// jr 	rs 			001000
			 	jr(instructions, data, op1, registers, pc, HiLo);
			 	break;
			case 9:		// jalr rd, rs 		001001
			 	jalr(instructions, data, op1, dest_reg, registers, pc, HiLo);
			 	break;
			//invalid instruction
			default: exit(-12);
		}
	}

		//filter funct 0x1_
	else if (funct_code < 0x20){
		switch(funct_code) {
			case 16:	//mfhi 	rd 			010000
			 	mfhi(dest_reg, registers, HiLo[0]);
			 	break;
			case 17: 	//mthi 	rs 			010001
			 	mthi(op1, registers, HiLo);
			 	break;
			case 18: 	//mflo 	rd 			010010
				mflo(dest_reg, registers, HiLo[1]);
			 	break;
			case 19:	//mtlo 	rs 			010011
			 	mtlo(op1, registers, HiLo);
			 	break;
			case 24:	//mult 	rs, rt 		011000
			 	mult(op1, op2, registers, HiLo);
			 	break;
			case 25: 	//multu rs, rt 		011001
			 	multu(op1, op2, registers, HiLo);
			 	break;
			case 26: 	//div 	rs, rt 		011010
			 	div(op1, op2, registers, HiLo);
			 	break;
			case 27: 	//divu 	rs, rt 		011011
			 	divu(op1, op2, registers, HiLo);
			 	break;
			//invalid instruction
			default: exit(-12);				 
		}
	}

		//filter funct 0x2_
	else{
		switch(funct_code) {
			case 32:	//add 	rd, rs, rt 	100000
				add(dest_reg, op1, op2, registers);
				break;
			case 33:	//addu 	rd, rs, rt 	100001
			 	addu(dest_reg, op1, op2, registers);
			 	break;
			case 34:	//sub 	rd, rs, rt 	100010
			 	sub(dest_reg, op1, op2, registers);
			 	break;
			case 35:	//subu 	rd, rs, rt 	100011
			 	subu(dest_reg, op1, op2, registers);
			 	break;
			case 36:	//and 	rd, rs, rt 	100100
				AND(dest_reg, op1, op2, registers);
			 	break;
			case 37:	//or 	rd, rs, rt 	100101
			 	OR(dest_reg, op1, op2, registers);
			 	break;
			case 38: //xor 	rd, rs, rt 	100110
			 	XOR(dest_reg, op1, op2, registers);
			 	break;
			case 39:	//nor 	rd, rs, rt 	100111
			 	NOR(dest_reg, op1, op2, registers);
			 	break;
			case 42: //slt 	rd, rs, rt 	101010
			 	slt(dest_reg, op1, op2, registers);
			 	break;
			case 43: //sltu 	rd, rs, rt 	101011
			 	sltu(dest_reg, op1, op2, registers);
			 	break;
			//invalid instruction
			default: exit(-12);				 
		}
	}
}

void execute_I (uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t &opcode){
	uint32_t dest_reg, src_reg;
	int32_t immediate;
	decode_fields_I(dest_reg, src_reg, immediate, instr);

	//filter 0x0_
	if (opcode < 0x10){
		switch(opcode){
			// case 1:		// CORNER CASE --> MULTIPLE POSSIBILITY
			// 	// bltz 	rs, label 	000001 		
			//	if (dest_reg == 0) bltz(---);
			// 	// bgez 	rs, label 	000001
			// 	else if (dest_reg == 1) bgez (---);
			//	// bltzal	re, label	000001
			//	else if (dest_reg == 16) bltzal(---);
			//	// bgezal	rs, ladel	000001
			//	else if (dest reg == 17) bgezal(---);
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
			//invalid instruction
			default: exit(-12);
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
			// case 34:	//lwl	rt, imm(rs)		100010
			//	lwl(---);
			//	break;
			// case 35:	//lw 	rt, imm(rs) 	100011
			// 	lw(---);
			// 	break;
			// case 36:	//lbu 	rt, imm(rs) 	100100
			// 	lbu(---);
			// 	break;
			// case 37:	//lhu 	rt, imm(rs) 	100101 
			// 	lhu(---);
			// 	break;
			// case 38:	//lwr 	rt, imm(rs) 	100110 
			// 	lwr(---);
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
			//invalid instruction
			default: exit(-12);
		}
	}	
}

void decode_fields_I (uint32_t &dest_reg, uint32_t &src_reg, int32_t& immediate, const uint32_t &instruction){
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


/////R TYPE INSTRUCTIONS//////

void sll(uint32_t dest_reg, uint32_t op, uint32_t shift_amt, int32_t (&registers)[32]){
	registers[dest_reg] = registers[op] << shift_amt;
}

void srl(uint32_t dest_reg, uint32_t op, uint32_t shift_amt, int32_t (&registers)[32]){
	registers[dest_reg] = registers[op] >> shift_amt;
}

void sra(uint32_t dest_reg, uint32_t op, uint32_t shift_amt, int32_t (&registers)[32]){
	int32_t num = registers[op];
	//if num is positive, operation is same as srl
	if (num >= 0) registers[dest_reg] = num >> shift_amt;
	//else duplicate sign bit
	else registers[dest_reg] = (num >> shift_amt) | (0xFFFFFFFF << 32 - 1);
}

void sllv(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	registers[dest_reg] = registers[op2] << registers[op1];
}

void srlv(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	registers[dest_reg] = registers[op2] >> registers[op1];
}

void srav(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	int32_t num = registers[op2];
	int32_t shift_amt = registers[op1];
	//if num is positive, operation is same as srl
	if (num >= 0) registers[dest_reg] = num >> shift_amt;
	//else duplicate sign bit
	else registers[dest_reg] = (num >> shift_amt) | (0xFFFFFFFF << 32 - 1);
}

void jr(vector <uint32_t> &instructions, uint8_t* data, uint32_t src_reg, int32_t (&registers)[32], uint32_t& pc, int32_t (&HiLo)[2]){
	pc++;
	execute(instructions, data, registers, pc, HiLo);
	if (src_reg != 0 && registers[src_reg] != 0) pc = (registers[src_reg] / 4) - 1;
	else pc = instructions.size() - 1;
}

void jalr(vector <uint32_t> &instructions, uint8_t* data, uint32_t src_reg, uint32_t dest_reg, int32_t (&registers)[32], uint32_t& pc, int32_t (&HiLo)[2]){
	//save return address
	registers[dest_reg] = pc + 2;
	//execute jr
	jr(instructions, data, src_reg, registers, pc, HiLo);
}

void mfhi(uint32_t dest_reg, int32_t (&registers)[32], int32_t HI){
	registers[dest_reg] = HI;
}

void mthi(uint32_t src_reg, int32_t (&registers)[32], int32_t (&HiLo)[2]){
	HiLo[0] = registers[src_reg];
}

void mflo(uint32_t dest_reg, int32_t (&registers)[32], int32_t LO){
	registers[dest_reg] = LO;
}

void mtlo(uint32_t src_reg, int32_t (&registers)[32], int32_t (&HiLo)[2]){
	HiLo[1] = registers[src_reg];
}

void mult(uint32_t op1, uint32_t op2, int32_t (&registers)[32], int32_t (&HiLo)[2]){
	int64_t product = registers[op1] * registers[op2];
	//put higher word in HI and lower word in LO
	HiLo[0] = product >> 32;
	HiLo[1] = product & 0x0000FFFF;
}

void multu(uint32_t op1, uint32_t op2, int32_t (&registers)[32], int32_t (&HiLo)[2]){
	uint64_t product = (uint32_t)registers[op1] * (uint32_t)registers[op2];
	//put higher word in HI and lower word in LO
	HiLo[0] = product >> 32;
	HiLo[1] = product & 0x0000FFFF;
}

void div(uint32_t op1, uint32_t op2, int32_t (&registers)[32], int32_t (&HiLo)[2]){
	int32_t dividend = registers[op1];
	int32_t divisor = registers[op2];
	//division by 0 --> arithmetic exception
	//if (divisor == 0) exit(-10);
	int32_t rem = dividend % divisor;
	int32_t quot = (dividend - rem) / divisor;
	//store quotient in LO and remainder in HI
	HiLo[0] = rem;
	HiLo[1] = quot;
}

void divu(uint32_t op1, uint32_t op2, int32_t (&registers)[32], int32_t (&HiLo)[2]){
	uint32_t dividend = registers[op1];
	uint32_t divisor = registers[op2];
	//division by 0 --> arithmetic exception
	//if (divisor == 0) exit(-10);
	uint32_t rem = dividend % divisor;
	uint32_t quot = (dividend - rem) / divisor;
	//store quotient in LO and remainder in HI
	HiLo[0] = rem;
	HiLo[1] = quot;
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

void addu(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	registers[dest_reg] = registers[op1] + registers[op2];
}

void sub(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	//sub same as add with 2's complement
	int32_t source1 = registers[op1];
	//get 2's complement of op2
	int32_t source2 = ~(registers[op2]) + 1;
	int64_t diff = source1 + source2;
	//check for signed/unsigned overflow
	int32_t diff2 = source1 + source2;
	if (diff != diff2) exit(-10);
	else registers[dest_reg] =  diff2;
}

void subu(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	registers[dest_reg] = registers[op1] - registers[op2];
}

void AND(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	registers[dest_reg] = registers[op1] & registers[op2];
}

void OR(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	registers[dest_reg] = registers[op1] | registers[op2];
}

void XOR(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	registers[dest_reg] = registers[op1] ^ registers[op2];
}

void NOR(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	registers[dest_reg] = ~(registers[op1] | registers[op2]);
}

void slt(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	if (registers[op1] < registers[op2]) registers[dest_reg] = 1;
	else registers[dest_reg] = 0;
}

void sltu(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	uint32_t a = registers[op1];
	uint32_t b = registers[op2];
	if (a < b) registers[dest_reg] = 1;
	else registers[dest_reg] = 0;
}



//////I TYPE INSTRUCTIONS///////

void addi(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]){
	int32_t source = registers[src_reg];
	int64_t sum = source + immediate;
	int32_t sum2 = source + immediate;
	if (sum != sum2) exit(-10);
	else registers[dest_reg] = sum2;
}

void addiu(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]){
	registers[dest_reg] = registers[src_reg] + immediate;
}

void lui(uint32_t &dest_reg, int32_t &immediate, int32_t (&registers)[32]){
	registers[dest_reg] = immediate << 16;
}

void sb(uint32_t address, uint8_t* data, uint8_t value){
		//check if mem to be accessed is between correct bounds for data space
	if (address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE){
		//remove data offset and write
		data[address - ADDR_DATA] = value;
	}
	//else check if instruction is trying to write ADDR_PUTC location
	else if (address == 0x30000004){
		cout << (char) value << endl;
	}
	else if (address > 0x30000004 && address < 0x30000008) return;
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



//////J TYPE INSTRUCTIONS///////

void j(vector <uint32_t> &instructions, uint8_t* data, int32_t (&registers)[32], uint32_t& pc, int32_t (&HiLo)[2]){
	uint32_t instr = instructions[pc];
	uint32_t instr_index = (instr << 6) >> 6;
	pc++;
	execute(instructions, data, registers, pc, HiLo);
	uint32_t temp = pc << 2;
	pc = (((temp & 0xF0000000) + (instr_index << 2)) >> 2) - 1;
}

void jal(vector <uint32_t> &instructions, uint8_t* data, int32_t (&registers)[32], uint32_t& pc, int32_t (&HiLo)[2]){
	//link return address in register 31
	registers[31]  = pc + 2;
	//execute normal j
	j (instructions, data, registers, pc, HiLo);
}