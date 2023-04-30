#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "gdt.h"
#include "hwcomm/interrupts.h"
#include "iolib.h"
#include "drivers/keyboard.h"
#include "multitasking/processmanager.h"
#include "drivers/mouse.h"


namespace microkernel
{
    class Kernel : public InterruptHandler
    {
    public:
        Kernel();
        ~Kernel();

        void run();

        void sys_waitpid(CPUState* t_cpu_state);
        CPUState* sys_exit(CPUState* t_cpu_state);
        CPUState* sys_execve(CPUState* t_cpu_state);
        void sys_fork(CPUState* t_cpu_state);

        virtual uint32_t handle_interrupt(uint32_t esp);

        static uint16_t fork();
        static void exit();
        static int16_t waitpid(uint16_t pid, ProcessState status);
        static int execve(void (*t_func_pointer)(void*), void* t_argv);

        static uint16_t get_current_pid();

        static void get_processes();

        static uint32_t schedule(uint32_t esp);

    private:
        GlobalDescriptorTable* gdt;
        InterruptManager* interrupt_manager;
        ProcessManager* process_manager;
        MemManager* ss_manager; // Stack Segment Manager
        KeyboardDriver* keyboard_driver;
        MouseDriver* mouse_driver;

        static Kernel* active_kernel;
    };

    extern "C" void init();
    
} // namespace microkernel


#endif
