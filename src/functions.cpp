int execute(u8& mem[RAM_SIZE], int& pc) {

	//get instruction from 4 bytes in mem
	u32 instruction = mem[pc];
	for (int x = 1; x < 4; x++){
		instruction = instruction << 8;
		instruction += mem[pc + x];
	}

	//decode instruction and call correct subfunction
	//isolate opcode
	unsigned char opcode = instruction >> 26;
	//check if R type
	if (opcode == 0)
		//execute
		execute_R(mem, instruction);
	//check if J type
	else if (opcode == 2 || opcode == 3){
		//execute
		execute_J(mem, instruction, opcode);
	}
	//further decode and (eventually) execute
	else execute_I(mem, instruction, opcode);
}

