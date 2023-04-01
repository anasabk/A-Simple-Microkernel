#ifndef __KERNEL_H__
#define __KERNEL_H__


#include "gdt.h"
#include "hwcomm/interrupts.h"
#include "iolib.h"
#include "drivers/keyboard.h"

namespace microkernel
{
    using namespace hwcomm;

    class Kernel
    {
    public:
        Kernel(/* args */);
        ~Kernel();

        void run();

    private:
        GlobalDescriptorTable gdt;
        InterruptManager interrupt_manager;
    };
    
} // namespace microkernel


#endif
