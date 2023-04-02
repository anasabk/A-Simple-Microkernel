#ifndef __KERNEL_H__
#define __KERNEL_H__


#include "gdt.h"
#include "hwcomm/interrupts.h"
#include "iolib.h"
#include "drivers/keyboard.h"
#include "multitasking/taskmanager.h"


namespace microkernel
{
    class Kernel
    {
    public:
        Kernel(/* args */);
        ~Kernel();

        void run();

    private:
        GlobalDescriptorTable* gdt;
        InterruptManager* interrupt_manager;
        Taskmanager* task_manager;
        MemManager* mem_manager;

    };
    
} // namespace microkernel


#endif
