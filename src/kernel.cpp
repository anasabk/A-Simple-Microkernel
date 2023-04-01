#include "kernel.h"


using namespace microkernel;

Kernel::Kernel()
    : gdt(), interrupt_manager(0x20, &gdt)
{
}

Kernel::~Kernel()
{
}

void Kernel::run() 
{
    printf("Initializing gdt\n");
    gdt.init();
    printf("Finished initialization.\n");

    printf("activating interrupt manager\n");
    interrupt_manager.activate();
    printf("finished activating\n");
    
    while(1);
}
