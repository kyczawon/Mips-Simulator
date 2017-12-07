#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <stdint.h>
#include <cmath>
#include <iostream>
#include "constants.hpp"
#include "functions.hpp"
using namespace std;
bool debug_mode;


int main(int argc, char* argv[]){
	//debug mode
	if (argc == 3) debug_mode = (strcmp(argv[2],"-d")==0);
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
	//program counter for jumps/branches
	uint32_t pc_next = 1;
	 //HI, LO special registers
	int32_t HiLo[2] = {0};


// //////////////////OLD WAY --> BOOOORING (and incorrect)
// 	//open Binary file
// 	ifstream infile;
// 	infile.open(argv[1]);


// 	//if IO error exit with correct exit code
// 	if (!infile.is_open()) {
// 		exit(-21);
// 	}

// 	//load instructions from Binary file into correct location in RAM 
// 	string input;
// 	int32_t offset = ADDR_INSTR;
// 	while (infile >> input) {
// 		uint32_t x = bin_string_to_uint32_t(input);
// 		instructions.push_back(x);
// 	}

// ///////////////////////////////////////////////////////////////////////	



/////////////////NEW WAY --> 100% SWAG
	streampos size;
	char * memblock;
	//open file with 'binary' attributes
  	ifstream infile (argv[1], ios::in|ios::binary|ios::ate);
	//if IO error exit with correct exit code
	if (!infile.is_open()) exit(-21);
	//else read the file and store in temporary dinamically allocated array of bytes
	size = infile.tellg();
	memblock = new char [size];
	infile.seekg (0, ios::beg);
	infile.read (memblock, size);
	infile.close();
	//combine bytes and copy instructions in vector
	uint32_t temp;
	for(int i = 0; i < size; i += 4){
		temp = 0;
		//combine adjacent bytes
		for(int j = 0; j < 4; j++){
			temp = temp | ((uint8_t)memblock[i + j] << (8 * (3 - j)));
		}
		instructions.push_back(temp);
	}
	//delete temporary array of bytes
   	delete[] memblock;
		////	(this version of the I/O protocol with the input file has been implemented later on
		//// 	but all the simulator relies on the instructions to be stored as 32b words in a vector
		//// 	that is why we kept the vector as final data structure)



	//execute instructions
	while (pc < instructions.size()){
		execute(instructions, data, registers, pc, pc_next, HiLo);
		//if 'non-pc-related' instruction has been executed
		if (pc_next == 0) break; //because jr sets pc_next to zero 
		else if (pc_next == pc + 1){
			//update pc & pc_next as usual
			pc++;
			pc_next++;
			//if (pc > instructions.size()) exit(-11);
		}				
		//if branch/jump has been executed
		else{
			if (debug_mode) cout << "entered pc update" << pc << endl;
			//check that there is an instruction to be executed!
			//if (pc + 1 > instructions.size()) exit(-11);
			//execute next instruction because of delay slot
			execute(instructions, data, registers, ++pc, pc_next, HiLo);
			//update pc to point to new instruction
			pc = pc_next;
			pc_next = pc + 1;
		}
	}
	char exit_result = (char) registers[2];
  exit(exit_result);
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

void execute(vector <uint32_t> &instructions, uint8_t* data, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next, int32_t (&HiLo)[2]) {
	uint32_t instr = instructions[pc];
	//decode instruction and call correct subfunction
	//isolate opcode
	uint8_t opcode = instr >> 26;
	//check if R type
	if (opcode == 0){
		//execute
		execute_R(instr, data, registers, pc, pc_next, HiLo);
	}
	//check if J type
	else if (opcode == 2 || opcode == 3){
		//execute
		execute_J(instr, registers, opcode, pc, pc_next);
	}
	//further decode and (eventually) execute
	else execute_I(instr, data, registers, opcode, pc, pc_next);
}

void execute_J(uint32_t instr, int32_t (&registers)[32], uint8_t& opcode, uint32_t& pc, uint32_t& pc_next) {
	uint32_t instr_index = (instr << 6) >> 6;
	
	//Jump (J)
	if (opcode == 2){ 
		j(instr_index, pc, pc_next);
	}
	//Jump and Link (JAL)
	else if (opcode == 3){
		jal(instr_index, registers, pc, pc_next);
	}
	//invalid instruction
	else exit(-12);
}

void execute_R(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next, int32_t (&HiLo)[2]) {
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
			case 2:		// srl 	rd, rt, sa 	000010
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
			 	jr(op1, registers, pc, pc_next);
			 	break;
			case 9:		// jalr rd, rs 		001001
			 	jalr(op1, dest_reg, registers, pc, pc_next);
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

void execute_I (uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t &opcode, uint32_t& pc, uint32_t& pc_next){
	uint32_t dest_reg, src_reg;
	int32_t immediate;
	decode_fields_I(dest_reg, src_reg, immediate, instr);

	//filter 0x0_
	if (opcode < 0x10){
		switch(opcode){
			case 1:		// CORNER CASE --> MULTIPLE POSSIBILITY
			 	// bltz 	rs, label 	000001 		
				if (dest_reg == 0) bltz(src_reg, immediate, registers, pc, pc_next);
			 	// bgez 	rs, label 	000001
				else if (dest_reg == 1) bgez (src_reg, immediate, registers, pc, pc_next);
				// bltzal	re, label	000001
				else if (dest_reg == 16) bltzal(src_reg, immediate, registers, pc, pc_next);
				// bgezal	rs, ladel	000001
				else if (dest_reg == 17) bgezal(src_reg, immediate, registers, pc, pc_next);
			 	break;
			case 4:		// beq	rs, rt, label 		000100 
			 	beq(src_reg, dest_reg, immediate, registers, pc, pc_next);
			 	break;
			case 5:		// bne 	rs, rt, label 		000101
			 	bne(src_reg, dest_reg, immediate, registers, pc, pc_next);
			 	break;
			case 6:		// blez 	rs, label 		000110 
			 	blez(src_reg, immediate, registers, pc, pc_next);
			 	break;
			case 7:		// bgtz 	rs, label 		000111
			 	bgtz(src_reg, immediate, registers, pc, pc_next);
			 	break;
			case 8:		// addi 	rt, rs, imm 		001000 
				addi(dest_reg, src_reg, immediate, registers);
				break;
			case 9:		// addiu 	rt, rs, imm 		001001
				addiu(dest_reg, src_reg, immediate, registers); 
				break;
			case 10:	// slti 	rt, rs, imm 		001010 
			 	slti(dest_reg, src_reg, immediate, registers);
			 	break;
			case 11:	// sltiu 	rt, rs, imm 		001011
			 	sltiu(dest_reg, src_reg, immediate, registers);
			 	break;
			case 12:	// andi 	rt, rs, imm 		001100
			 	andi(dest_reg, src_reg, immediate, registers); 
			 	break;
			case 13:	// ori 	rt, rs, imm 			001101
			 	ori(dest_reg, src_reg, immediate, registers);
			 	break;
			case 14:	// xori 	rt, rs, imm 		001110 
			 	xori(dest_reg, src_reg, immediate, registers);
			 	break;
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
			case 32:	//lb 	rt, imm(rs) 	100000
			 	lb(registers[src_reg] + immediate, data, dest_reg, registers);
			 	break;
			case 33:	//lh 	rt, imm(rs) 	100001
			 	lh(registers[src_reg] + immediate, data, dest_reg, registers);
			 	break;
			case 34:	//lwl	rt, imm(rs)		100010
				lwl(registers[src_reg] + immediate, data, dest_reg, registers);
				break;
			case 35:	//lw 	rt, imm(rs) 	100011
			 	lw(registers[src_reg] + immediate, data, dest_reg, registers);
			 	break;
			case 36:	//lbu 	rt, imm(rs) 	100100
			 	lbu(registers[src_reg] + immediate, data, dest_reg, registers);
			 	break;
			case 37:	//lhu 	rt, imm(rs) 	100101 
			 	lhu(registers[src_reg] + immediate, data, dest_reg, registers);
			 	break;
			case 38:	//lwr 	rt, imm(rs) 	100110 
			 	lwr(registers[src_reg] + immediate, data, dest_reg, registers);
			 	break;
			case 40:	//sb 	rt, imm(rs) 	101000
				sb(registers[src_reg] + immediate, data, registers[dest_reg] & 0x0000FF);
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
	else exit(-12);
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
	registers[dest_reg] = ((uint32_t) registers[op]) >> shift_amt;
}

void sra(uint32_t dest_reg, uint32_t op, uint32_t shift_amt, int32_t (&registers)[32]){
	int32_t num = registers[op];
	//if num is positive, operation is same as srl
	if (num >= 0) registers[dest_reg] = num >> shift_amt;
	//else duplicate sign bit
	else registers[dest_reg] = (num >> shift_amt) | (0xFFFFFFFF << (32 - shift_amt));
}

void sllv(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	registers[dest_reg] = registers[op2] << registers[op1];
}

void srlv(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	registers[dest_reg] = ((uint32_t) registers[op2]) >> registers[op1];
}

void srav(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]){
	int32_t num = registers[op2];
	int32_t shift_amt = registers[op1];
	//if num is positive, operation is same as srl
	if (num >= 0) registers[dest_reg] = num >> shift_amt;
	//else duplicate sign bit
	else registers[dest_reg] = (num >> shift_amt) | (0xFFFFFFFF << 32 - 1);
}

void jr(uint32_t src_reg, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next){
	pc_next = registers[src_reg];
}

void jalr(uint32_t src_reg, uint32_t dest_reg, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next){
	//save return address
	registers[dest_reg] = pc + 2;
	//execute jr
	jr(src_reg, registers, pc, pc_next);
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
	if (debug_mode) cout << registers[op1] << "*" << registers[op2] << endl;
	int64_t product = (int64_t)registers[op1] * (int64_t)registers[op2];
	//put higher word in HI and lower word in LO
	HiLo[1] = (int32_t) product;
	HiLo[0] = (int32_t) (product >> 32);
}

void multu(uint32_t op1, uint32_t op2, int32_t (&registers)[32], int32_t (&HiLo)[2]){
	if (debug_mode) cout << (uint64_t)registers[op1] << "*" << (uint64_t)registers[op2] << endl;
	uint64_t product = (uint64_t) ((uint32_t)registers[op1]) * (uint64_t)((uint32_t)registers[op2]);
	//put higher word in HI and lower word in LO
	HiLo[1] = (int32_t) product;
	HiLo[0] = (int32_t) (product >> 32);
	if (debug_mode) cout << "product " << product << " hi" << HiLo[0] << " lo" << HiLo[1] << endl;
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
	uint32_t dividend = (uint32_t)registers[op1];
	uint32_t divisor = (uint32_t)registers[op2];
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
	if (debug_mode) cout << "source1 " << source1 << "source2 " << source2 << endl;
	uint64_t sum = source1 + source2;
	//check for signed/unsigned overflow
	int32_t sum2 = source1 + source2;
	if (debug_mode) cout << "sum " << sum << "sum2 " << sum2 << endl;
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
	int32_t source2 = registers[op2];

	int64_t diff = source1 - source2;
	//check for signed/unsigned overflow
	int32_t diff2 = source1 - source2;

	if (debug_mode) cout<< source1 << "-" << source2 << " = " << diff2 << endl;
	if (diff != diff2) exit(-10);
	else{
		registers[dest_reg] =  diff2;
	}
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
	uint32_t a = (uint32_t) registers[op1];
	uint32_t b = (uint32_t) registers[op2];
	if (a < b) registers[dest_reg] = 1;
	else registers[dest_reg] = 0;
}



//////I TYPE INSTRUCTIONS///////

void bltz(uint32_t &op, int32_t &immediate, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next){
	if (debug_mode) cout << "registers[op]" << registers[op] << endl;;
	if (registers[op] < 0){
		pc_next += immediate;
	}
}

void bgez(uint32_t &op, int32_t &immediate, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next){
	if (registers[op] >= 0){
		pc_next += immediate;
	}
}

void bltzal(uint32_t &op, int32_t &immediate, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next){
	if (registers[op] < 0){
		registers[31] = pc + 2;
		pc_next += immediate;
	}
}

void bgezal(uint32_t &op, int32_t &immediate, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next){
	if (registers[op] >= 0){
		registers[31] = pc + 2;
		pc_next += immediate;
	}
}

void beq(uint32_t &op1, uint32_t &op2, int32_t &immediate, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next){
	if (debug_mode) cout << "immediate: " << immediate << endl;
	if (debug_mode) cout << "registers[op1]: " << registers[op1] << "registers[op2]: " << registers[op2] << immediate << endl;
	if (registers[op1] == registers[op2]){
		pc_next += immediate;
		if (debug_mode) cout << "entered:" << endl;
	}
	if (debug_mode) cout << "pc_next: " << pc_next << endl;
}

void bne(uint32_t &op1, uint32_t &op2, int32_t &immediate, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next){
	if (debug_mode) cout << "registers[op1]: " << registers[op1] << "registers[op2]: " << registers[op2] << immediate << endl;
	if (registers[op1] != registers[op2]){
		pc_next += immediate;
	}
}

void blez(uint32_t &op, int32_t &immediate, int32_t (&registers)[32],  uint32_t& pc, uint32_t& pc_next){
	if (registers[op] <= 0){
		pc_next += immediate;
	}
}

void bgtz(uint32_t &op, int32_t &immediate, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next){
	if (registers[op] > 0){
		pc_next += immediate;
	}
}

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

void slti(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]){
	if (registers[src_reg] < immediate) registers[dest_reg] = 1;
	else registers[dest_reg] = 0;
}

void sltiu(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]){
	if ((uint32_t)registers[src_reg] < (uint32_t)immediate) registers[dest_reg] = 1;
	else registers[dest_reg] = 0;
}

void andi(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]){
	registers[dest_reg] = registers[src_reg] & immediate;
}

void ori(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]){
	if (debug_mode) cout << "entered ori" << endl;
	immediate = immediate & 0x0000FFFF;
	registers[dest_reg] = registers[src_reg] | immediate;
}

void xori(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]){
	registers[dest_reg] = registers[src_reg] ^ immediate;
}

void lui(uint32_t &dest_reg, int32_t &immediate, int32_t (&registers)[32]){
	registers[dest_reg] = immediate << 16;
}

void lb(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]){
		//check if mem to be accessed is between correct bounds for data space
	if (address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE){
		//remove data offset and read
		int8_t temp = data[address - ADDR_DATA];
		//check if sign extension is needed
		if (temp >= 0) registers[dest_reg] = temp;
		else registers[dest_reg] = (int32_t)temp;
	}
	//else check if instruction is trying to read ADDR_GETC location
	else if (address == 0x30000000){
		//read from keyboard
		cin >> registers[dest_reg];
	}
	//if other ADDR_GETC locations don't to anything
	else if (address > 0x30000000 && address < 0x30000004) return;
	//otherwise return error code
	else exit(-11);
}

void lh(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]){
	//check for address alignment
	if (address % 2 != 0) exit(-11);
	//check if mem to be accessed is between correct bounds for data space
	if (address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE){
		//remove offset to address
		address -= ADDR_DATA;
		//load higher byte
		int16_t temp = 0 | ((uint16_t)data[address] << 8);
		//load lower byte
		temp = temp | (uint16_t)(data[address + 1]);
		//check if sign extension is needed
		if (temp >= 0) registers[dest_reg] = temp;
		else registers[dest_reg] = (int32_t)temp;
	}
	//else check if instruction is trying to read ADDR_GETC location
	else if (address == 0x30000000){
		//read from keyboard
		cin >> registers[dest_reg];
	}
	//if other ADDR_GETC locations don't to anything
	else if (address == 0x30000002) return;
	//otherwise return error code
	else exit(-11);

}
	
void lwl(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]){
	//check if mem to be accessed is between correct bounds for data space
	if (!(address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE)) exit(-11);
	//infer unalignment from address
	uint32_t unalignment = 4 - (address % 4);
	//if no unalignment execute normal load word
	if (unalignment == 4){
		lw(address, data, dest_reg, registers);
		return;
	}
	//apply offset to address
	address -= ADDR_DATA;
	//load unaligned data
	uint32_t temp = 0x0;
	uint32_t past_val = registers[dest_reg];
	past_val = (past_val << (8 * unalignment)) >> (8 * unalignment);
	for(int x = 0; x < unalignment; x++){
		temp = temp | ((uint32_t)data[address + x] << (8 * (3-x)));
	}
	registers[dest_reg] = past_val | temp;
	
}

void lw(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]){
	//check for address alignment
	if (address % 4 != 0) exit(-11);
	//check if mem to be accessed is between correct bounds for data space
	if (address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE){
		//remove offset to address
		address -= ADDR_DATA;
		//load highest byte
		registers[dest_reg] = (uint32_t)data[address] << 24;
		if (debug_mode) cout << "address" << ": " << (uint32_t)data[address] << endl;
		//load lower bytes
		for (int x = 1; x < 4; x++){
			if (debug_mode) cout << "address +" << x << ": " << (uint32_t)data[address + x] << endl;
			if (debug_mode) cout << "registers[dest_reg] value: " << registers[dest_reg] << endl;
			registers[dest_reg] = registers[dest_reg] | ((uint32_t)data[address + x] << (8 * (3 - x)));
		}
	}
	//else check if instruction is trying to read ADDR_GETC location
	else if (address == 0x30000000){
		//read from keyboard
		cin >> registers[dest_reg];
	}
	//otherwise return error code
	else exit(-11);

}

void lbu(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]){
		//check if mem to be accessed is between correct bounds for data space
	if (address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE){
		//remove data offset and read
		uint32_t temp = data[address - ADDR_DATA];
		registers[dest_reg] = temp & 0x000000FF;
	}
	//else check if instruction is trying to read ADDR_GETC location
	else if (address == 0x30000000){
		//read from keyboard
		uint32_t temp;
		cin >> temp;
		registers[dest_reg] = temp;
	}
	//if other ADDR_GETC locations don't to anything
	else if (address > 0x30000000 && address < 0x30000004) return;
	//otherwise return error code
	else exit(-11);
}

void lhu(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]){
	//check for address alignment
	if (address % 2 != 0) exit(-11);
	//check if mem to be accessed is between correct bounds for data space
	if (address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE){
		//remove offset to address
		address = address - ADDR_DATA;
		//load higher byte
		int32_t temp = 0 | ((uint32_t)data[address] << 8);
		//load lower byte
		temp = temp | (uint32_t)(data[address + 1]);
		registers[dest_reg] = temp & 0x0000FFFF;
	}
	//else check if instruction is trying to read ADDR_GETC location
	else if (address == 0x30000000){
		//read from keyboard
		cin >> registers[dest_reg];
	}
	//if other ADDR_GETC locations don't to anything
	else if (address == 0x30000002) return;
	//otherwise return error code
	else exit(-11);

}

void lwr(int32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]){
	//check if mem to be accessed is between correct bounds for data space
	if (!(address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE)) exit(-11);
	//infer unalignment from address
	uint32_t unalignment = address % 4;
	//if no unalignment execute normal load word
	if (unalignment == 3){
		lw(address - 3, data, dest_reg, registers);
		return;
	}
	//apply offset to address
	address -= ADDR_DATA;
	//load unaligned data
	uint32_t temp = 0x0;
	uint32_t past_val = registers[dest_reg];
	past_val = (past_val >> (8 * ++unalignment)) << (8 * unalignment);
	for(int x = 0; x <= unalignment; x++){
		temp = temp | ((uint32_t)data[address - x] << (8 * x));
	}
	registers[dest_reg] = past_val | temp;
}

void sb(uint32_t address, uint8_t* data, uint8_t value){
		//check if mem to be accessed is between correct bounds for data space
	if (address >= ADDR_DATA && address < ADDR_DATA + DATA_SIZE){
		//remove data offset and write
		data[address - ADDR_DATA] = value;
		if (debug_mode) cout << "value: " << (uint32_t) value << endl;
	}
	//else check if instruction is trying to write ADDR_PUTC location
	else if (address == 0x30000007){
		cout << value << endl;
	}
	else if (address > 0x30000003 && address < 0x30000007) return;
	//otherwise return error code
	else exit(-11);
}

void sh(uint32_t address, uint8_t* data, int32_t value){
	if (address % 2 != 0) exit(-11);
	uint8_t lower_byte, higher_byte;
	lower_byte = (uint8_t) value & 0x000000FF;
	sb(address + 1, data, lower_byte);
	higher_byte = (value & 0x0000FF00) >> 8;
	sb(address, data, higher_byte);
}

void sw(uint32_t address, uint8_t* data, int32_t value){
	if (debug_mode) cout << "sw value: " << value << endl;
	if (address % 4 != 0) exit(-11);
	int32_t lower_half, higher_half;
	lower_half = value & 0x0000FFFF;
	sh(address + 2, data, lower_half);
	higher_half = (value & 0xFFFF0000) >> 16;
	sh(address, data, higher_half);
}



//////J TYPE INSTRUCTIONS///////

void j(uint32_t& instr_index, uint32_t& pc, uint32_t& pc_next){
	uint32_t temp = pc << 2;
	pc_next = (((temp & 0xF0000000) | (instr_index << 2)) >> 2);
	if (debug_mode) cout << "instr_index" << instr_index << endl;
}

void jal(uint32_t& instr_index, int32_t (&registers)[32], uint32_t& pc, uint32_t& pc_next){
	if (debug_mode) cout << "entered jal" << endl;
	//link return address in register 31
	registers[31]  = pc + 2;
	//execute normal j
	j (instr_index, pc, pc_next);
}

void print_registers(int32_t (&registers)[32]){
	if (debug_mode) {
		for (int i=0; i< 32;i++) {
			cout << "register " << i << ": " << registers[i] << endl;
		}
	}
}