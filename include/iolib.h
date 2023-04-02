#ifndef __IOLIB_H__
#define __IOLIB_H__

#include "common/types.h"


using namespace microkernel;

void printf(char* str);

void clearf();

void printf_int(uint32_t num);

void printfHex(uint8_t key);

void printfHex16(uint16_t key);

void printfHex32(uint32_t key);

void printfHex64(uint64_t key);


#endif
