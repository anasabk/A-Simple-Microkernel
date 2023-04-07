#ifndef __KERNEL_H__
#define __KERNEL_H__


#include "gdt.h"
#include "hwcomm/interrupts.h"
#include "iolib.h"
#include "drivers/keyboard.h"
#include "multitasking/taskmanager.h"


namespace microkernel
{
    class Kernel : public InterruptHandler
    {
    public:
        Kernel();
        ~Kernel();

        void run();

        Task* create_task(void* t_entry_point, uint32_t t_stack_size);

        void fork_task(uint32_t esp, void* t_return_addr);
        void vfork_task(uint32_t esp, void* t_return_addr);
        void execve_task(uint32_t esp, const char* pathname);

        virtual uint32_t handle_interrupt(uint32_t esp);


        static volatile uint16_t fork();
        static volatile uint16_t vfork();
        static void sysprintf(char* str);
        static int execve(const char *pathname, char *const argv[], char *const envp[]);

        static uint16_t get_pid();

    private:
        GlobalDescriptorTable gdt;
        InterruptManager interrupt_manager;
        Taskmanager task_manager;
        MemManager mem_manager;
        Keyboard keyboard_driver;

        static Kernel* active_kernel;
    };
    
} // namespace microkernel


#endif
