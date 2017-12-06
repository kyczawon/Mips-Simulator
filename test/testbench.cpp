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
    
    if (argc > 1) debug_mode = (strcmp(argv[1],"-d")==0);
    ofstream output;
    string output_name = "test/output/output.csv";
    output.open(output_name.c_str());
    if (!output.is_open()) {
        cout << "Output file could not be opened" << endl;
        exit(EXIT_FAILURE);
    }

    test_jr(test_id, debug_mode, output);
    test_R_and_I(test_id, debug_mode, output);
    test_sl(test_id, debug_mode, output);
    test_muldiv(test_id, debug_mode, output);
    test_branch(test_id, debug_mode, output);
    test_link_fwd(test_id, debug_mode, output);
}

void test_jr(int& test_id, bool debug_mode, ofstream& output){
    ofstream binary;
    string binary_name = "test/temp/binary.bin";
    binary.open(binary_name.c_str());
    if (!binary.is_open()) {
        cout << "binary file could not be created" << endl;
        exit(EXIT_FAILURE);
    }
    //ORI $v0 $v0 0x3631
    binary << "00110100010000100011011000110001" << endl;
    //jr $zero
    binary << "00000000000000000000000000001000" << endl;
    binary.close();
    int32_t result=0, exit_code=0;
    get_simulator_output(debug_mode, result, exit_code);
    string status = "Fail";
    stringstream message;
    message << ",[jr $zero expected: -207 got: " << exit_code << "]";

    if (exit_code == -207) {
        status = "Pass";
    }
    cout << test_id << "| jr" << ", " << status << ", Alelo " << message.str() << endl;
    output << test_id << "| jr" << ", " << status << ", Alelo " << message.str() << endl;
}

void test_link_fwd(int& test_id, bool debug_mode, ofstream& output){
    ofstream binary;
    ifstream instructions;
    string instructions_name = "test/link_instructions.txt";
    instructions.open(instructions_name.c_str());
    if (!instructions.is_open()) {
        cout << "link_instructions.txt file not found" << endl;
        exit(EXIT_FAILURE);
    }
    //load instructions in the format instr | name | input1 | input2 | expected_result (1 = branch, 2= no branch) | expected_exit_code
    //instr must have the offset set to 
    string instr, instr_name;
    int input1, input2, expected_result, expected_exit;
    while (instructions >> instr >> instr_name >> input1 >> input2 >> expected_result >> expected_exit) {

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
        //lui s1 - 16 most significant bits of input 1
        ss << "0011110000010001" << input1_binary.substr(0, 16) << endl;
        //ori s1 s1 - 16 least significant bits of input 1
        ss << "0011011000110001" << input1_binary.substr(16, 32) << endl;
        //instruction to test
        ss << instr << endl;
        //addi $s2 $s2 0x1
        ss << "00100010010100100000000000000001" << endl;
        //addi $s2 $s2 0x1
        ss << "00100010010100100000000000000001" << endl;
        //-----------put word to output-----------
        //lui t0 0x3000
        ss << "00111100000010000011000000000000" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //jr $zero
        ss << "00000000000000000000000000001000" << endl;
        //addi $s2 $s2 0x1
        ss << "00100010010100100000000000000001" << endl;
        //jr $ra
        ss << "00000011111000000000000000001000";
        binary << ss.str();
        binary.close();
        
        int32_t result=0, exit_code=0;
        get_simulator_output(debug_mode, result, exit_code);

        string status = "Fail";
        stringstream message;
        message << ",[ " << input1 << " " << instr_name << " " << input2 << " expected the branch to";
        if (expected_result == 2) {
            message << " not";
        }
        message << " execute with exit code: " << expected_exit;
        if (((expected_result == 2 && result == 2) || (expected_result == 1 && result == 3)) && exit_code == expected_exit) {
            status = "Pass";
        }

        cout << test_id << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
        output << test_id++ << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
    }
    instructions.close();
}

// void test_link_back(int& test_id, bool debug_mode, ofstream& output){
//     ofstream binary;
//     ifstream instructions;
//     string instructions_name = "test/link_back_instructions.txt";
//     instructions.open(instructions_name.c_str());
//     if (!instructions.is_open()) {
//         cout << "link_back_instructions.txt file not found" << endl;
//         exit(EXIT_FAILURE);
//     }
//     //load instructions in the format instr | name | input1 | input2 | expected_result (1 = branch, 2= no branch)
//     //instr must have the offset set to 
//     string instr, instr_name;
//     int input1, input2, expected_result;
//     while (instructions >> instr >> instr_name >> input1 >> input2 >> expected_result) {

//         string binary_name = "test/temp/binary.bin";
//         binary.open(binary_name.c_str());
//         if (!binary.is_open()) {
//             cout << "binary file could not be created" << endl;
//             exit(EXIT_FAILURE);
//         }
//         //convert input as int to binary string
//         string input1_binary = int_to_bin(input1);
//         string input2_binary = int_to_bin(input2);
        
//         stringstream ss;
//         //lui s1 - 16 most significant bits of input 1
//         ss << "0011110000010001" << input1_binary.substr(0, 16) << endl;
//         //ori s1 s1 - 16 least significant bits of input 1
//         ss << "0011011000110001" << input1_binary.substr(16, 32) << endl;
//         //Beq $zero $zero 0x3
//         ss << "00010000000000000000000000000011" << endl;
//         //addi $s3 $s3 0x0
//         ss << "00100010011100110000000000000000" << endl;
//         //addi $v0 $v0 0x1
//         ss << "00100000010000100000000000000001" << endl;
//         //jr $ra
//         ss << "00000011111000000000000000001000" << endl;
//         //instruction to test
//         ss << instr << endl;
//         //addi $v0 $v0 0x1
//         ss << "00100000010000100000000000000001" << endl;
//         //addi $v0 $v0 0x1
//         ss << "00100000010000100000000000000001" << endl;
//         //jr $zero
//         ss << "00000000000000000000000000001000";
//         binary << ss.str();
//         binary.close();
        

            // int32_t result=0, exit_code=0;
            // get_simulator_output(debug_mode, result, exit_code);

//         string status = "Fail";
//         stringstream message;
//         message << ",[ " << input1 << " " << instr_name << " " << input2 << " expected the branch to";
//         if (expected_result == 2) {
//             message << " not";
//         }
//         message << " execute] " << result;
//         if ((expected_result == 2 && result == 2) || (expected_result == 1 && result == 3)) {
//             status = "Pass";
//         }

//         cout << test_id << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
//         output << test_id++ << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
//     }
//     instructions.close();
// }

void test_branch(int& test_id, bool debug_mode, ofstream& output){
    ofstream binary;
    ifstream instructions;
    string instructions_name = "test/brj_instructions.txt";
    instructions.open(instructions_name.c_str());
    if (!instructions.is_open()) {
        cout << "brj_instructions.txt file not found" << endl;
        exit(EXIT_FAILURE);
    }
    //load instructions in the format instr | name | input1 | input2 | expected_result (1 = branch, 2= no branch) | expected_exit
    string instr, instr_name;
    int input1, input2, expected_result, expected_exit;
    while (instructions >> instr >> instr_name >> input1 >> input2 >> expected_result >> expected_exit) {

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
        //lui s1 - 16 most significant bits of input 1
        ss << "0011110000010001" << input1_binary.substr(0, 16) << endl;
        //ori s1 s1 - 16 least significant bits of input 1
        ss << "0011011000110001" << input1_binary.substr(16, 32) << endl;
        //lui s0 - 16 most significant bits of input 2
        ss << "0011110000010000" << input2_binary.substr(0, 16) << endl;
        //ori s0 s0 - 16 least significant bits of input 2
        ss << "0011011000010000" << input2_binary.substr(16, 32) << endl;
        //instruction to test with offset 2
        ss << instr << "0000000000000010" << endl;
        //addi $s2 $s2 0x1
        ss << "00100010010100100000000000000001" << endl;
        //addi $s2 $s2 0x1
        ss << "00100010010100100000000000000001" << endl;
        //-----------put word to output-----------
        //lui t0 0x3000
        ss << "00111100000010000011000000000000" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //jr $zero
        ss << "00000000000000000000000000001000";
        binary << ss.str();
        binary.close();
        
       
        int32_t result=0, exit_code=0;
        get_simulator_output(debug_mode, result, exit_code);

        string status = "Fail";
        stringstream message;
        message << ",[ " << input1 << " " << instr_name << " " << input2 << " expected the branch to execute " << expected_result << " instruction(s) following the branch. The branch executed: "
        << result << "| Expected exit_code: " << expected_exit << " exit: " << exit_code;
        if (instr_name == "beq") {
            message << "| IF THIS INSTRUCTION FAILS, ALL BACKWARD BRANCHES WILL FAIL";
        } 
        message << "]";
        if (result == expected_result && exit_code == expected_exit) {
            status = "Pass";
        }

        cout << test_id << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
        output << test_id++ << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
        
        binary.open(binary_name.c_str());
        if (!binary.is_open()) {
            cout << "binary file could not be created" << endl;
            exit(EXIT_FAILURE);
        }

        ss.str( std::string() );
        ss.clear();
        
        //lui s1 - 16 most significant bits of input 1
        ss << "0011110000010001" << input1_binary.substr(0, 16) << endl;
        //ori s1 s1 - 16 least significant bits of input 1
        ss << "0011011000110001" << input1_binary.substr(16, 32) << endl;
        //lui s0 - 16 most significant bits of input 2
        ss << "0011110000010000" << input2_binary.substr(0, 16) << endl;
        //ori s0 s0 - 16 least significant bits of input 2
        ss << "0011011000010000" << input2_binary.substr(16, 32) << endl;
        //Beq $zero $zero 0x10
        ss << "00010000000000000000000000001010" << endl;
        //addi $s3 $s3 0x0
        ss << "00100010011100110000000000000000" << endl;
        //-----------put word to output-----------
        //lui t0 0x3000
        ss << "00111100000010000011000000000000" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //jr $zero
        ss << "00000000000000000000000000001000" << endl;
        //instruction to test with offset -10
        ss << instr << "1111111111110110" << endl;
        //addi $s2 $s2 0x1
        ss << "00100010010100100000000000000001" << endl;
        //addi $s2 $s2 0x1
        ss << "00100010010100100000000000000001" << endl;
        //-----------put word to output-----------
        //lui t0 0x3000
        ss << "00111100000010000011000000000000" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //srl s2 s2 0x0008
        ss << "00000000000100101001001000000010" << endl;
        //sb s2 0x0004 t0
        ss << "10100001000100100000000000000100" << endl;
        //jr $zero
        ss << "00000000000000000000000000001000";
        binary << ss.str();
        binary.close();
        
        get_simulator_output(debug_mode, result, exit_code);

        status = "Fail";
        message.str( std::string() );
        message.clear();
        message << ",[ " << input1 << " " << instr_name << " " << input2 << " expected the branch to execute " << expected_result << " instruction(s) following the branch. The branch executed: "
        << result << "| Expected exit_code: " << expected_exit << " exit: " << exit_code;
        if (result == expected_result && exit_code == expected_exit) {
            status = "Pass";
        }

        cout << test_id << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
        output << test_id++ << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
    }
    instructions.close();
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
        //load instructions in the format instr | name | input1 | input2 | result | exit
        string instr, instr_name;
        int input1, input2, expected_result, expected_exit;
        while (instructions >> instr >> instr_name >> input1 >> input2 >> expected_result >> expected_exit) {

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
            //lui t0 0x3000
            ss << "00111100000010000011000000000000" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            binary << ss.str();
            binary.close();
            
            int32_t result=0, exit_code=0;
            get_simulator_output(debug_mode, result, exit_code);

            string status = "Fail";
            stringstream message;
            message << ",[ " << input1 << " " << instr_name << " " << input2 << " expected: " << expected_result << " got: "
            << result << "| Expected exit_code " << expected_exit << " got: " << exit_code << "]";

            if (result == expected_result && exit_code == expected_exit) {
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

        //store/load instructions in the format instr | name | address | expected_result | expected_exit_code | is_store
        string instr, instr_name, hex_load_address, hex_store_address;
        int input1, expected_result, expected_exit, is_store;
        while (instructions >> instr >> instr_name >> hex_store_address >> hex_load_address >> input1 >> expected_result >> expected_exit >> is_store) {
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

            int store_address_init = store_address;
            while (store_address_init % 4 != 0) {
                store_address_init--;
            }
            string store_address_init_binary = int_to_bin(store_address_init);
            
            stringstream ss;
            //-----------put msb of address into register accessed by store-----------
            //lui v1 16 most significant bits of address
            ss << "0011110000000011" << store_address_binary.substr(0, 16) << endl;

            //-----------initialize the word to -1-----------
            //lui s1 - 16 most significant bits of input 1
            ss << "00111100000100011111111111111111" << endl;
            //ori s1 s1 - 16 least significant bits of input 1
            ss << "00110110001100011111111111111111" << endl;
            //sw $s1 address (16lsb) v1 (address 16msb)
            ss << "1010110001110001" << store_address_init_binary.substr(16, 32) << endl;

            //-----------put value to be stored-----------
            //lui s0 - 16 most significant bits of input 1
            ss << "0011110000010000" << input1_binary.substr(0, 16) << endl;
            //ori s0 s0 - 16 least significant bits of input 1
            ss << "0011011000010000" << input1_binary.substr(16, 32) << endl;
            
            //-----------test store and then load word-----------
            if (!is_store) { //if load, store a value and test load
                //lui s2 - 16 most significant bits of input 1
                ss << "00111100000100101111111111111111" << endl;
                //ori s2 s2 - 16 least significant bits of input 1
                ss << "00110110010100101111111111111111" << endl;
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
            //lui t0 0x3000
            ss << "00111100000010000011000000000000" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            binary << ss.str();
            binary.close();
            
            
            int32_t result=0, exit_code=0;
            get_simulator_output(debug_mode, result, exit_code);

            string status = "Fail";
            stringstream message;
            string hexaddress;
            if (is_store) hexaddress = hex_store_address;
            else  hexaddress = hex_load_address;
            message << ",[ " << instr_name << " " << input1 << " " << hexaddress << " expected: " << expected_result
            << " got: " << result << "| Expected exit: " << expected_exit << " got: " << exit_code << "]";

            if (result == expected_result && exit_code == expected_exit) {
                status = "Pass";
            }

            cout << test_id << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
            output << test_id++ << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
            
        }
        instructions.close();
}

void test_muldiv(int& test_id, bool debug_mode, ofstream& output){
    ofstream binary;
    ifstream instructions;
    string instructions_name = "test/muldiv_instructions.txt";
        instructions.open(instructions_name.c_str());
        if (!instructions.is_open()) {
            cout << "muldiv_instructions file not found" << endl;
            exit(EXIT_FAILURE);
        }
        //load instructions in the format instr | name | input1 | input2 | hi_result | lo_result | expected_exit
        string instr, instr_name;
        int input1, input2, expected_hi_result, expected_lo_result, expected_exit;
        while (instructions >> instr >> instr_name >> input1 >> input2 >> expected_hi_result >> expected_lo_result >> expected_exit) {

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
            ss << "0011110000010001" << input2_binary.substr(0, 16) << endl;
            //ori s1 s1 - 16 least significant bits of input 1
            ss << "0011011000110001" << input2_binary.substr(16, 32) << endl;
            //lui s0 - 16 most significant bits of input 2
            ss << "0011110000010000" << input1_binary.substr(0, 16) << endl;
            //ori s0 s0 - 16 least significant bits of input 2
            ss << "0011011000010000" << input1_binary.substr(16, 32) << endl;
            //instruction to test
            ss << "00000010000100010000000000" << instr << endl;

            //mfhi s2
            ss << "00000000000000001001000000010000" << endl;

            //-----------put word s2 to output-----------
            //lui t0 0x3000
            ss << "00111100000010000011000000000000" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;

            binary << ss.str();
            binary.close();
            
            
            int32_t result=0, exit_code=0;
            get_simulator_output(debug_mode, result, exit_code);

            string status = "Fail";
            stringstream message;
            message << ",[ " << input1 << " " << instr_name << " from high " << input2 << " expected: " << expected_hi_result << " got: "
            << result << "| Expected exit_code " << expected_exit << " got: " << exit_code << "]";

            if (result == expected_hi_result && exit_code == expected_exit) {
                status = "Pass";
            }

            cout << test_id << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
            output << test_id++ << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;

            binary.open(binary_name.c_str());
            if (!binary.is_open()) {
                cout << "binary file could not be created" << endl;
                exit(EXIT_FAILURE);
            }

            ss.str( std::string() );
            ss.clear();

            //-----------initialize registers with required values-----------
            //lui s1 - 16 most significant bits of input 1
            ss << "0011110000010001" << input2_binary.substr(0, 16) << endl;
            //ori s1 s1 - 16 least significant bits of input 1
            ss << "0011011000110001" << input2_binary.substr(16, 32) << endl;
            //lui s0 - 16 most significant bits of input 2
            ss << "0011110000010000" << input1_binary.substr(0, 16) << endl;
            //ori s0 s0 - 16 least significant bits of input 2
            ss << "0011011000010000" << input1_binary.substr(16, 32) << endl;
            //instruction to test
            ss << "00000010000100010000000000" << instr << endl;

            //mflo s2
            ss << "00000000000000001001000000010010" << endl;
            //-----------put word s2 to output-----------
            //lui t0 0x3000
            ss << "00111100000010000011000000000000" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            //srl s2 s2 0x0008
            ss << "00000000000100101001001000000010" << endl;
            //sb s2 0x0004 t0
            ss << "10100001000100100000000000000100" << endl;
            
            binary << ss.str();
            binary.close();
            
            get_simulator_output(debug_mode, result, exit_code);

            status = "Fail";
            message.str( std::string() );
            message.clear();
            message << ",[ " << input1 << " " << instr_name << " from low " << input2 << " expected: " << expected_hi_result << " got: "
            << result << "| Expected exit_code " << expected_exit << " got: " << exit_code << "]";

            if (result == expected_lo_result && exit_code == expected_exit) {
                status = "Pass";
            }

            cout << test_id << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;
            output << test_id++ << ", " << instr_name << ", " << status << ", Alelo " << message.str() << endl;

        }
        instructions.close();
}

void get_simulator_output(bool debug_mode, int32_t& result, int32_t& exit_code) {
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

    result = 0;
    int count = 0;
    stringstream ss;
    while (fgets(output, 1024, fp) != NULL) {
        if (debug_mode) {
            ss << output << endl;
        } else {
            if (*output == '\n') {
                test_line_break(fp, output, result, count);
            } else {
                uint8_t num = (uint8_t) *output;
                result = result | (uint32_t)(num << 8 * count++);
            }
        }
    }
    if (debug_mode) cout << ss.str() << endl;
    status = pclose(fp);
    exit_code = WEXITSTATUS(status);
    if (exit_code != 0) exit_code-=256;
    if (status == -1) {
         cout << "error closing simulator" << endl;
    }
}

void test_line_break(FILE *fp, char* output, int32_t& result, int& count) {
    if (fgets(output, 1024, fp) != NULL) {
        if (*output == '\n') {
            result = result | (uint32_t)(10 << 8 * count++);
            test_line_break(fp, output, result, count);
        } else {
            uint8_t num = (uint8_t) *output;
            result = result | (uint32_t)(num << 8 * count++);
        }
    }
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