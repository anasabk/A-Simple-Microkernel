#ifndef __IOLIB_H__
#define __IOLIB_H__

#include "common/types.h"


namespace microkernel
{
    extern char in_buffer[256];
    extern int16_t buf_index;
    extern int8_t ready;

    void printf(char* str);

    void printf(char c);

    void clearf();

    void printf_int(int32_t num);

    void printfHex(uint8_t key);

    void printfHex16(uint16_t key);

    void printfHex32(uint32_t key);

    void printfHex64(uint64_t key);

    char getchar();

    void gets(char* buffer, uint8_t len);

} // namespace microkernel


#endif
