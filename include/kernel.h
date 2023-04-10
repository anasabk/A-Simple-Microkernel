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
        void sys_exit(uint32_t esp);

        virtual uint32_t handle_interrupt(uint32_t esp);

        static uint16_t fork();
        static void exit();
        // static volatile uint16_t vfork();
        static void sys_printf(char* str);
        static int execve(const char *pathname, char *const argv[], char *const envp[]);

        static uint16_t get_pid();

        static void dump_stack();

    private:
        GlobalDescriptorTable gdt;
        InterruptManager interrupt_manager;
        ProcessManager process_manager;
        MemManager ss_manager; // Stack Segment Manager
        // MemManager ts_manager; // Task Segment Manager
        Keyboard keyboard_driver;

        static Kernel* active_kernel;
    };
    
} // namespace microkernel


#endif
