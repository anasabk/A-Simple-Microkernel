#include "iolib.h"


uint16_t* VideoMemory = (uint16_t*)0xb8000;
uint8_t row = 0, column = 0;

void printf(char* str) 
{
    for(int i = 0; str[i] != '\0'; ++i){
        switch(str[i]) {
            case '\n':
                row = 0;
                column++;
                break;

            case '\b':
                if(row > 0) {
                    row--;
                }

                VideoMemory[80*column + row] = (VideoMemory[80*column + row] & 0xFF00) | ' ';
                break;

            default: 
                VideoMemory[80*column + row] = (VideoMemory[80*column + row] & 0xFF00) | str[i];
                row++;
                break;
        }

        if(row > 80) {
            column++;
            row = 0;
        }

        if(column >= 25) {
            // for(column = 0; column < 25; column++)
            //     for(row = 0; row < 80; row++)
            //         VideoMemory[80*column + row] = (VideoMemory[80*column + row] & 0xFF00) | ' ';
            // row = 0;
            // column = 0;
            column--;
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
    } else {
        int i;
        for(i = 0; temp > 0 && i < 32; i++) {
            buffer[i] = temp%10 + 48;
            temp /= 10;
        }

        for(i--; i >= 0; i--) {
            printf(buffer[i]);
        }
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
