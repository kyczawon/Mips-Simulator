#include <stdint.h>
#include <string>
using namespace std;
string int_to_bin(int32_t value);
void get_simulator_output(bool debug_mode, int32_t& result, int32_t& exit_code);
void test_R_and_I(int& test_id, bool debug_mode, ofstream& output);
void test_sl(int& test_id, bool debug_mode, ofstream& output);
void test_muldiv(int& test_id, bool debug_mode, ofstream& output);
void test_branch(int& test_id, bool debug_mode, ofstream& output);
void test_link_fwd(int& test_id, bool debug_mode, ofstream& output);
void test_link_back(int& test_id, bool debug_mode, ofstream& output);
void test_jr(int& test_id, bool debug_mode, ofstream& output);
void test_line_break(FILE *fp, char* output, int32_t& result, int& count);
char bin_string_to_char(string input);
void populate_vector(vector<char> &instr_bytes, string instruction);
void copy_vector_to_buffer(const vector<char> &instr_bytes, char* buffer);
void s2_to_output(vector<char>& instr_bytes);