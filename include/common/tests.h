#ifndef __TESTS_H__
#define __TESTS_H__

#include "types.h"
#include "iolib.h"
#include "kernel.h"

using namespace microkernel;

struct search_param {
    int32_t* arr;
    size_t length;
    int16_t target;
};

int32_t binary_search(int32_t* arr, size_t len, int32_t target);

int32_t linear_search(int32_t* arr, size_t len, int32_t target);

void collatz_sequence(uint32_t num);

void task_collatz_sequence(void* param);

void task_binary_search(void* param);

void task_linear_search(void* param);

#endif
