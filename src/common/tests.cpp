#include "common/tests.h"


int32_t binary_search(int32_t* arr, size_t len, int32_t target) {
    if(len == 0)
        return -1;

    if(arr[len/2] == target)
        return len/2;

    if(arr[len/2] > target)
        return binary_search(arr, len/2, target);
    else
        return len/2 + binary_search(&arr[len/2], len/2, target);
}

int32_t linear_search(int32_t* arr, size_t len, int32_t target) {
    for(int i = 0; i < len; i++)
        if(arr[i] == target)
            return i;

    return -1;
}

void collatz_sequence(uint32_t num) {
    printf_int(num);
    printf(" ");
    if(num == 1) {
        return;
    } else if(num % 2 == 0) {
        collatz_sequence(num/2);
    } else {
        collatz_sequence(num*3+1);
    }
}

int32_t str_to_int(char* str, uint16_t len) {
    int result = 0;
    for(int i = 0; i < len && str[i] <= '9' && str[i] >= '0'; i++)
        result = result*10 + str[i] - 48;

    return result;
}

void task_collatz_sequence(void* ignore) {
    printf("Collatz Sequence:\n");

    int32_t number = 0;
    char buffer[16];

    printf("Please enter x where the collatz sequence will be calculated for all numbers between 0 and x: ");
    gets(buffer, 16);

    number = str_to_int(buffer, 15);
    
    for(int32_t i = number; i > 0; i--) {
        printf_int(i);
        printf(": ");
        collatz_sequence(i);
        printf("\n");
    }

    Kernel::exit();
}

void task_binary_search(void* param) {
    printf("binary search: \n"); 
    char buffer[256];
    int32_t length;
    int32_t array[1024];
    int32_t target;

    printf("Enter the size of the array to be scaned: ");
    gets(buffer, 255);
    length = str_to_int(buffer, 255);

    printf("Enter the array to be scaned in the format \"number1 number2\" without quotes: ");
    gets(buffer, 255);

    int j = 0;
    for(int i = 0; i < length; i++)
    {
        array[i] = 0;
        
        if(buffer[j] <= '9' && buffer[j] >= '0')
            array[i] = str_to_int(&buffer[j], 255 - j);

        for(j++; j < 256 && !(buffer[j] <= '9' && buffer[j] >= '0'); j++);

        if(buffer[j] == 0)
            break;
    }

    printf("Enter the number to search for: ");
    gets(buffer, 255);
    target = str_to_int(buffer, 255);

    printf("Result: ");
    printf_int(binary_search(array, length, target));
    printf("\n");

    Kernel::exit();
}

void task_linear_search(void* param) {
    printf("linear search: ");
    char buffer[256];
    int32_t length;
    int32_t array[1024];
    int32_t target;

    printf("Enter the size of the array to be scaned: ");
    gets(buffer, 255);
    length = str_to_int(buffer, 255);

    printf("Enter the array to be scaned in the format \"number1 number2\" without quotes: ");
    gets(buffer, 255);

    int j = 0;
    for(int i = 0; i < length; i++)
    {
        array[i] = 0;
        
        if(buffer[j] <= '9' && buffer[j] >= '0')
            array[i] = str_to_int(&buffer[j], 255 - j);

        for(j++; j < 256 && !(buffer[j] <= '9' && buffer[j] >= '0'); j++);

        if(buffer[j] == 0)
            break;
    }

    printf("Enter the number to search for: ");
    gets(buffer, 255);
    target = str_to_int(buffer, 255);

    printf("Result: ");
    printf_int(linear_search(array, length, target));
    printf("\n");

    Kernel::exit();
}
