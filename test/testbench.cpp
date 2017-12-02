#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <stdint.h>
#include <cmath>
#include <sstream>
#include <stdio.h>
#include "testbench.hpp"
using namespace std;

int main(int argc, char* argv[])
{
    bool debug_mode = false;
    int test_id = 0;
    //debug mode
    
	if (argc > 1) debug_mode = (strcmp(argv[1], "d"));
    ofstream output;
    string output_name = "test/output/output.csv";
    output.open(output_name.c_str());
    if (!output.is_open()) {
        cout << "Output file could not be opened" << endl;
        exit(EXIT_FAILURE);
    }

    test_R_and_I(test_id, debug_mode, output);
    // test_sl(test_id, debug_mode, output);

}

void test_R_and_I(int& test_id, bool debug_mode, ofstream& output){
    ofstream binary;
    ifstream instructions;
    string instructions_name = "test/instructions.txt";
        instructions.open(instructions_name.c_str());
        if (!instructions.is_open()) {
            cout << "Instructions file not found" << endl;
            exit(EXIT_FAILURE);
        }
        //load instructions in the format instr | name | input1 | input2 | result 
        string instr, instr_name;
        int input1, input2, expected_result;
        while (instructions >> instr >> instr_name >> input1 >> input2 >> expected_result) {

            string binary_name = "test/temp/binary.bin";
            binary.open(binary_name.c_str());
            if (!binary.is_open()) {
                cout << "binary file could not be created" << endl;
                exit(EXIT_FAILURE);
            }
            //convert input as int to binary string
            string input1_binary = int_to_bin(input1);
            string input2_binary = int_to_bin(input2);
            
            stringstream ss;
            //-----------initialize registers with required values-----------
            //lui s1 - 16 most significant bits of input 1
            ss << "0011110000010001" << input1_binary.substr(0, 16) << endl;
            //ori s1 s1 - 16 least significant bits of input 1
            ss << "0011011000110001" << input1_binary.substr(16, 32) << endl;
            //lui s0 - 16 most significant bits of input 2
            ss << "0011110000010000" << input2_binary.substr(0, 16) << endl;
            //ori s0 s0 - 16 least significant bits of input 2
            ss << "0011011000010000" << input2_binary.substr(16, 32) << endl;
            //instruction to test
            ss << instr << endl;
            //-----------put word to output-----------
            //lui v0 0x3000
            ss << "00111100000000100011000000000000" << endl;
            //sb s2 0x0004 v0
            ss << "10100000010100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 v0
            ss << "10100000010100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 v0
            ss << "10100000010100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 v0
            ss << "10100000010100100000000000000100" << endl;
            binary << ss.str();
            binary.close();
            
            int32_t result = get_simulator_output(debug_mode);

            string status = "Fail";
            stringstream message;
            message << "[, " << input1 << " " << instr_name << " " << input2 << " expected: " << expected_result << ", got: " << result << "]";

            if (result == expected_result) {
                status = "Pass";
            }

            cout << test_id << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
            output << test_id++ << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
            
        }
        instructions.close();
}

void test_sl(int& test_id, bool debug_mode, ofstream& output){
    ofstream binary;
    ifstream instructions;
    string instructions_name = "test/sl_instructions.txt";
        instructions.open(instructions_name.c_str());
        if (!instructions.is_open()) {
            cout << "sl_instructions file not found" << endl;
            exit(EXIT_FAILURE);
        }

        //store/load instructions in the format instr | name | address | result | is_store
        string instr, instr_name, hex_load_address, hex_store_address;
        int input1, expected_result, is_store;
        while (instructions >> instr >> instr_name >> hex_load_address >> hex_store_address >> input1 >> expected_result >> is_store) {
            string binary_name = "test/temp/binary.bin";
            binary.open(binary_name.c_str());
            if (!binary.is_open()) {
                cout << "binary file could not be created" << endl;
                exit(EXIT_FAILURE);
            }

            int load_address = (int)strtol(hex_load_address.c_str(), NULL, 0);
            int store_address = (int)strtol(hex_store_address.c_str(), NULL, 0);
            string load_address_binary = int_to_bin(load_address);
            string store_address_binary = int_to_bin(store_address);
            string input1_binary = int_to_bin(input1);
            
            stringstream ss;
            //-----------put value to be stored-----------
            //lui s0 - 16 most significant bits of input 1
            ss << "0011110000010000" << input1_binary.substr(0, 16) << endl;
            //ori s0 s0 - 16 least significant bits of input 1
            ss << "0011011000010000" << input1_binary.substr(16, 32) << endl;

            //-----------put msb of address into register accessed by load-----------
            //lui v1 16 most significant bits of address
            ss << "0011110000000011" << load_address_binary.substr(0, 16) << endl;

            //-----------put msb of address into register accessed by store-----------
            //lui v1 16 most significant bits of address
            ss << "0011110000000011" << store_address_binary.substr(0, 16) << endl;

            //-----------test store and then load word-----------
            if (!is_store) { //if load, store a value and test load
                //sw $s0 address (16lsb) v1 (address 16msb)
                ss << "1010110001110000" << store_address_binary.substr(16, 32) << endl;
                //load_instruction to test with 16 least significant bits of address as immiediate
                //load_instruction $s2 address (16lsb) v1 (address 16msb)
                ss << instr << "0001110010" << load_address_binary.substr(16, 32) << endl;
            }
            if (is_store) { //if store, test store and load back
                //store_instruction to test with 16 least significant bits of address as immiediate
                //store_instruction $s0 address (16lsb) v1 (address 16msb)
                ss << instr << "0001110000" << store_address_binary.substr(16, 32) << endl;
                //lw $s2 address (16lsb) v1 (address 16msb)
                ss << "1000110001110010" << load_address_binary.substr(16, 32) << endl;
            }
            
            //-----------put word to output-----------
            //lui v0 0x3000
            ss << "00111100000000100011000000000000" << endl;
            //sb s2 0x0004 v0
            ss << "10100000010100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 v0
            ss << "10100000010100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 v0
            ss << "10100000010100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 v0
            ss << "10100000010100100000000000000100" << endl;
            binary << ss.str();
            binary.close();
            
            int32_t result = get_simulator_output(debug_mode);

            string status = "Fail";
            stringstream message;
            string hexaddress;
            if (is_store) hexaddress = hex_store_address;
            else  hexaddress = hex_load_address;
            message << "[, " << instr_name << " " << input1 << " " << hexaddress << " expected: " << expected_result << ", got: " << result << "]";

            if (result == expected_result) {
                status = "Pass";
            }

            cout << test_id << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
            output << test_id++ << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
            
        }
        instructions.close();
}

int32_t get_simulator_output(bool debug_mode) {
    FILE *fp;
    int status;
    char output[1024];

    string simulator_command = "bin/mips_simulator test/temp/binary.bin";

    if (debug_mode) simulator_command += " -d";

    fp = popen(simulator_command.c_str(), "r");

    /* Handle error */
    if (fp == NULL) {
        cout << "error opening simulator" << endl;
    }

    int32_t result = 0;
    int count = 0;
    stringstream ss;
    while (fgets(output, 1024, fp) != NULL) {
        if (debug_mode) {
            ss << output << endl;
        } else {
            uint8_t num = (uint8_t) *output;
            result = result | (uint32_t)(num << 8 * count++);
        }
    }
    if (debug_mode) cout << ss.str() << endl;

    status = pclose(fp);
    if (status == -1) {
         cout << "error closing simulator" << endl;
    }

    return result;
}

string int_to_bin(int value) {
    string bin ="0";
    //if value is negative the first bit is 1 and value needs to be updated
    if (value < 0) {
        bin = "1";
        value += pow(2, 31);
    }

    for (int i = 30; i >= 0; --i) {
        int power = pow(2, i);
        if (value >= power) {
            bin += "1";
            value -= power;
        } else {
            bin += "0";
        }
    }
    return bin;
}
