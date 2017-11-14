#include <vector>
#include <stdint.h>
using namespace std;

void execute(vector <uint32_t> &instructions, uint8_t* data, int32_t (&registers)[32] , uint32_t& pc);
void execute_R(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint32_t& pc, vector <uint32_t> &instructions);
void execute_J(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t& opcode, uint32_t& pc);
void execute_I(uint32_t instr, uint8_t* data, int32_t (&registers)[32], uint8_t& opcode);
void decode_fields_R (uint32_t &op1, uint32_t &op2, uint32_t &dest_reg, uint32_t &shift_amt, uint32_t &funct_code, const uint32_t &instr);
void decode_fields_I (int32_t &dest_reg, int32_t &src_reg, int32_t& immediate, const int32_t &instruction);

//R type instructions
void jr(vector <uint32_t> &instructions, uint8_t* data, uint32_t src_reg, int32_t (&registers)[32], uint32_t& pc);
void add(uint32_t dest_reg, uint32_t op1, uint32_t op2, int32_t (&registers)[32]);
void sll(uint32_t dest_reg, uint32_t op, uint32_t shift_amt, int32_t (&registers)[32]);

//I type instructions
void addi(int32_t &dest_reg, int32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]);
void addiu(int32_t &dest_reg, int32_t &src_reg, int32_t &immediate, int32_t (&registers)[32]);
void lui(int32_t &dest_reg, int32_t &immediate, int32_t (&registers)[32]);
void sb(uint32_t address, uint8_t* data, uint8_t value);
void sh(uint32_t address, uint8_t* data, int32_t value);
void sw(uint32_t address, uint8_t* data, int32_t value);

//helper functions
uint32_t bin_string_to_int(string input);