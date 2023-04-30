#ifndef __KERNEL_H__
#define __KERNEL_H__


#include "gdt.h"
#include "hwcomm/interrupts.h"
#include "iolib.h"
#include "drivers/keyboard.h"
#include "multitasking/processmanager.h"

// #define DEBUG_FLAG


namespace microkernel
{
    class Kernel : public InterruptHandler
    {
    public:
        Kernel();
        ~Kernel();

        void run();

        /**
         * @brief Create a new process and add it to the process
         * manager.
         * 
         * @param t_entry_point The pointer to the main function
         * of the program.
         * @param t_stack_size Stack size to allocate for the process.
         * @return New process PID.
         */
        uint8_t create_task(void* t_entry_point, uint32_t t_stack_size);

        uint8_t sys_fork(uint32_t esp, void* t_return_addr);
        // void sys_vfork(uint32_t esp, void* t_return_addr);
        CPUState* sys_execve(uint32_t esp, char *const pathname, char *const argv[]);
        int16_t sys_waitpid(int16_t pid, ProcessState status);
        void sys_exit();

        virtual uint32_t handle_interrupt(uint32_t esp);

        static uint16_t fork();
        static void exit();
        static int16_t waitpid(uint16_t pid, ProcessState status);
        static void sys_printf(char* str);
        static int execve(void* pathname, void* argv);

        static uint16_t get_current_pid();

        static void dump_stack();

        static void get_processes();

    private:
        GlobalDescriptorTable gdt;
        InterruptManager interrupt_manager;
        ProcessManager process_manager;
        MemManager ss_manager; // Stack Segment Manager
        Keyboard keyboard_driver;

        static Kernel* active_kernel;
    };
    
} // namespace microkernel


#endif
