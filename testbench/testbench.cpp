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
    ofstream binary;
    ifstream instructions;

    string instructions_name = "testbench/instructions.txt";
    instructions.open(instructions_name.c_str());
    if (!instructions.is_open()) {
        cout << "Instructions file not found" << endl;
        exit(EXIT_FAILURE);
    }

    string binary_name = "testbench/binary.bin";
    binary.open(binary_name.c_str());
    if (!binary.is_open()) {
        cout << "binary file could not be created" << endl;
        exit(EXIT_FAILURE);
    }

    //load instructions in the format inst | name | input1 | input2 | result 
    string instr, instr_name, input1, input2, result;
    while (instructions >> instr >> instr_name >> input1 >> input2 >> result) {
        //convert input to int
        int value = stoi(input1);
        int value2 = stoi(input2);
        //convert input as int to binary string
        string input1_binary = int_to_bin(value);
        string input2_binary = int_to_bin(value2);
        
        stringstream ss;
        //lui s0 16 most significant bits of input 1
        ss << "0011110000010000" << input1_binary.substr(0, 16) << endl;
        //addiu s0 zero 16 least significant bits of input 1
        ss << "0010010000010000" << input1_binary.substr(16, 32) << endl;
        //lui s1 16 most significant bits of input 2
        ss << "0011110000010001" << input2_binary.substr(0, 16) << endl;
        //addiu s1 zero 16 least significant bits of input 2
        ss << "0010010000010001" << input2_binary.substr(16, 32) << endl;
        //instruction to test
        ss << instr << endl;
        //lui v0 0x3000
        ss << "00111100000000100011000000000000" << endl;
        //sw s2 0x0004 v0
        ss << "10101100010100100000000000000100" << endl;
        binary << ss.str();
    }
    instructions.close();
    binary.close();
    
    get_simulator_output();
    

}

uint32_t get_simulator_output() {
    FILE *fp;
    int status;
    char output[PATH_MAX];

    char buffer [100];
    int cx;

    fp = popen("bin/mips_simulator testbench/binary.bin", "r");

    /* Handle error */;
    if (fp == NULL) {
        cout << "error" << endl;
    }

    stringstream ss;
    while (fgets(output, PATH_MAX, fp) != NULL) {
        ss << output;
    }

    string str = ss.str();

    str.erase(remove(str.begin(), str.end(), '\n'), str.end());

    cout << str << endl;

    status = pclose(fp);
    if (status == -1) {
        /* Error reported by pclose() */
    } else {
        /* Use macros described under wait() to inspect `status' in order
        to determine success/failure of command executed by popen() */
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