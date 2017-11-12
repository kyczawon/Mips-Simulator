#include <vector>
using namespace std;
void execute(unsigned int instr, unsigned char* data, int (&registers)[32] , unsigned int& pc);
void execute_R(unsigned int instr, unsigned char* data, int (&registers)[32]);
void execute_J(unsigned int instr, unsigned char* data, int (&registers)[32], unsigned char& opcode, unsigned int& pc);
void execute_I(unsigned int instr, unsigned char* data, int (&registers)[32], unsigned char& opcode);
void decode_fields_R (unsigned int &dest_reg, unsigned int &op1, unsigned int &op2, unsigned int &shift_amt, unsigned int &funct_code, const unsigned int &instr);

//R type instructions
void add(unsigned int dest_reg, unsigned int op1, unsigned int op2, int (&registers)[32]);

//helper functions
unsigned int bin_string_to_int(string input);


