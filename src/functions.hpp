int execute(vector <unsigned char> &instructions, unsigned char (&data)[DATA_SIZE], unsigned char (&registers)[32] , int& pc);
void execute_R(unsigned char (&mem)[RAM_SIZE], unsigned int& instruction);
void execute_J(unsigned char (&mem)[RAM_SIZE], unsigned int& instruction, unsigned char& opcode);
void execute_I(unsigned char (&mem)[RAM_SIZE], unsigned int& instruction, unsigned char& opcode);


