#include <stdint.h>
#include <string>
using namespace std;
string int_to_bin(int32_t value);
int32_t get_simulator_output(bool debug_mode);
void test_R_and_I(int& test_id, bool debug_mode, ofstream& output);
void test_sl(int& test_id, bool debug_mode, ofstream& output);
void test_muldiv(int& test_id, bool debug_mode, ofstream& output);
void test_branch(int& test_id, bool debug_mode, ofstream& output);
void test_link_fwd(int& test_id, bool debug_mode, ofstream& output);
void test_link_back(int& test_id, bool debug_mode, ofstream& output);
void test_jr(int& test_id, bool debug_mode, ofstream& output);
void test_line_break(FILE *fp, char* output, int32_t& result, int& count);