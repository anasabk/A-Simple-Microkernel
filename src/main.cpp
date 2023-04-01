#include "kernel.h"
#include "iolib.h"


using namespace microkernel;

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void kernelMain (const void* multiboot_structure, uint32_t magic_number)
{
    printf("OS booting\n");

    Kernel kernel;
    kernel.run();
}