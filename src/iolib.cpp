#include "iolib.h"


namespace microkernel
{
    uint16_t* VideoMemory = (uint16_t*)0xb8000;
    uint8_t row = 0, column = 0, half = 0;

    char in_buffer[256];
    int16_t buf_index;
    int8_t ready;

    void printf(char* str) 
    {
        for(int i = 0; str[i] != '\0'; ++i){
            switch(str[i]) {
                case '\n':
                    column = half*40;
                    row++;
                    break;

                case '\b':
                    if(column > 0) {
                        column--;
                    }

                    VideoMemory[80*row + column] = (VideoMemory[80*row + column] & 0xFF00) | ' ';
                    break;

                default: 
                    VideoMemory[80*row + column] = (VideoMemory[80*row + column] & 0xFF00) | str[i];
                    column++;
                    break;
            }

            if(column >= (40 + half*40)) {
                row++;
                column = half*40;
            }

            if(row >= 25) {
                half = half ? 0 : 1;
                for(row = 0; row < 25; row++)
                    for(column = half*40; column < 40 + half*40; column++)
                        VideoMemory[80*row + column] = (VideoMemory[80*row + column] & 0xFF00) | ' ';

                row = 0;
                column = half*40;
            }
        }
    }

    void printf(char c) 
    {
        char buffer[2] = {c, 0};
        printf(buffer);
    }

    void clearf() {
        for(int i = 0; i < 80; ++i)
            for(int j = 0; j < 25; j++)
                VideoMemory[80*j + i] = ' ';

        row = 0;
        column = 0;
    }

    void printf_int(int32_t num) {
        int32_t temp = num;
        char buffer[32];

        if(temp < 0) {
            temp *= -1;
            printf("-");
        } else if(temp == 0) {
            printf(48);
            return;
        } 
        
        int i;
        for(i = 0; temp > 0 && i < 32; i++) {
            buffer[i] = temp%10 + 48;
            temp /= 10;
        }

        for(i--; i >= 0; i--) {
            printf(buffer[i]);
        }
    }

    void printfHex(uint8_t key)
    {
        char* foo = "00";
        char* hex = "0123456789ABCDEF";
        foo[0] = hex[(key >> 4) & 0xF];
        foo[1] = hex[key & 0xF];
        printf(foo);
    }

    void printfHex16(uint16_t key)
    {
        printfHex((key >> 8) & 0xFF);
        printfHex( key & 0xFF);
    }

    void printfHex32(uint32_t key)
    {
        printfHex16((key >> 16) & 0xFFFF);
        printfHex16( key & 0xFFFF);
    }

    void printfHex64(uint64_t key) {
        printfHex32((key >> 32) & 0xFFFFFFFF);
        printfHex32(key & 0xFFFFFFFF);
    }

    char getchar() {
        while(ready < 1);
        char target = in_buffer[buf_index];
        in_buffer[buf_index] = 0;
        buf_index--;
        ready = 0;
        return target;
    }

    void gets(char* buffer, uint8_t len) {
        while(ready < 1);

        int16_t start_i = 0;
        if(len <= buf_index)
            start_i = buf_index - len;

        for(int16_t i = 0; i < len; i++)
            buffer[i] = in_buffer[start_i + i];

        buf_index = start_i - 1;        
        in_buffer[buf_index + 1] = 0;
        ready = 0;
        
    }

} // namespace microkernel
