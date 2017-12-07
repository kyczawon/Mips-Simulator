#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <stdint.h>
const uint32_t DATA_SIZE  = 0x04000000;
const uint32_t INSTR_SIZE = 0x01000000;  
const uint32_t ADDR_INSTR = 0x10000000;
const uint32_t ADDR_DATA  = 0x20000000;
const uint32_t ADDR_GETC  = 0x30000000;
const uint32_t ADDR_PUTC  = 0x30000004;
#endif