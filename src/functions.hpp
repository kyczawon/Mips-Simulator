#include <vector>
#include <stdint.h>
using namespace std;

//main functions

void execute(vector <uint32_t> &instructions, uint8_t* data, int32_t (&registers)[32] , uint32_t& pc, int32_t (&HiLo)[2]);
void execute_R(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint32_t& pc, vector <uint32_t> &instructions, int32_t (&HiLo)[2]);
void execute_J(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t& opcode, uint32_t& pc, vector<uint32_t> &instructions, int32_t (&HiLo)[2]);
void execute_I(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t& opcode, uint32_t& pc, vector<uint32_t> &instructions, int32_t (&HiLo)[2]);
void decode_fields_R (uint32_t &op1, uint32_t &op2, uint32_t &dest_reg, uint32_t &shift_amt, uint32_t &funct_code, const uint32_t &instr);
void decode_fields_I (uint32_t &dest_reg, uint32_t &src_reg, int32_t& immediate, const uint32_t &instruction);

//R type instructions

void sll(uint32_t dest_reg, uint32_t op, uint32_t shift_amt, int32_t (&registers)[32]);
void srl(uint32_t dest_reg, uint32_t op, uint32_t shift_amt, int32_t (&registers)[32]);
void sra(uint32_t dest_reg, uint32_t op, uint32_t shift_amt, int32_t (&registers)[32]);
void sllv(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void srlv(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void srav(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void jr(vector <uint32_t> &instructions, uint8_t* data, uint32_t src_reg, int32_t (&registers)[32], uint32_t& pc, int32_t (&HiLo)[2]);
void jalr(vector <uint32_t> &instructions, uint8_t* data, uint32_t src_reg, uint32_t dest_reg, int32_t (&registers)[32], uint32_t& pc, int32_t (&HiLo)[2]);
void mfhi(uint32_t dest_reg, int32_t (&registers)[32], int32_t HI);
void mthi(uint32_t src_reg, int32_t (&registers)[32], int32_t (&HiLo)[2]);
void mflo(uint32_t dest_reg, int32_t (&registers)[32], int32_t LO);
void mtlo(uint32_t src_reg, int32_t (&registers)[32], int32_t (&HiLo)[2]);
void mult(uint32_t op1, uint32_t op2, int32_t (&registers)[32], int32_t (&HiLo)[2]);
void multu(uint32_t op1, uint32_t op2, int32_t (&registers)[32], int32_t (&HiLo)[2]);
void div(uint32_t op1, uint32_t op2, int32_t (&registers)[32], int32_t (&HiLo)[2]);
void divu(uint32_t op1, uint32_t op2, int32_t (&registers)[32], int32_t (&HiLo)[2]);
void add(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void addu(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void sub(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void subu(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void AND(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void OR(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void XOR(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void NOR(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void slt(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void sltu(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);

//I type instructions
void bltz(uint32_t &op, int32_t &immediate, int32_t (&registers)[32], vector <uint32_t> &instructions, uint8_t* data, uint32_t& pc, int32_t (&HiLo)[2]);
void bgez(uint32_t &op, int32_t &immediate, int32_t (&registers)[32], vector <uint32_t> &instructions, uint8_t* data, uint32_t& pc, int32_t (&HiLo)[2]);
void bltzal(uint32_t &op, int32_t &immediate, int32_t (&registers)[32], vector <uint32_t> &instructions, uint8_t* data, uint32_t& pc, int32_t (&HiLo)[2]);
void bgezal(uint32_t &op, int32_t &immediate, int32_t (&registers)[32], vector <uint32_t> &instructions, uint8_t* data, uint32_t& pc, int32_t (&HiLo)[2]);
void beq(uint32_t &op1, uint32_t &op2, int32_t &immediate, int32_t (&registers)[32], vector <uint32_t> &instructions, uint8_t* data, uint32_t& pc, int32_t (&HiLo)[2]);
void bne(uint32_t &op1, uint32_t &op2, int32_t &immediate, int32_t (&registers)[32], vector <uint32_t> &instructions, uint8_t* data, uint32_t& pc, int32_t (&HiLo)[2]);
void blez(uint32_t &op, int32_t &immediate, int32_t (&registers)[32], vector <uint32_t> &instructions, uint8_t* data, uint32_t& pc, int32_t (&HiLo)[2]);
void bgtz(uint32_t &op, int32_t &immediate, int32_t (&registers)[32], vector <uint32_t> &instructions, uint8_t* data, uint32_t& pc, int32_t (&HiLo)[2]);
void addi(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]);
void addiu(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]);
void slti(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]);
void sltiu(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]);
void andi(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]);
void ori(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]);
void xori(uint32_t &dest_reg, uint32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]);
void lui(uint32_t &dest_reg, int32_t &immediate, int32_t (&registers)[32]);
void lb(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]);
void lh(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]);
void lwl(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]); 
void lw(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]);
void lbu(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]);
void lhu(uint32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]);
void lwr(int32_t address, uint8_t* data, uint32_t dest_reg, int32_t (&registers)[32]);
void sb(uint32_t address, uint8_t* data, uint8_t value);
void sh(uint32_t address, uint8_t* data, int32_t value);
void sw(uint32_t address, uint8_t* data, int32_t value);

//J type instructions
void j(vector <uint32_t> &instructions, uint8_t* data, int32_t (&registers)[32], uint32_t& pc, int32_t (&HiLo)[2]);
void jal(vector <uint32_t> &instructions, uint8_t* data, int32_t (&registers)[32], uint32_t& pc, int32_t (&HiLo)[2]);

//helper functions
uint32_t bin_string_to_uint32_t(string input);