#ifndef __PROCESSMANAGER_H__
#define __PROCESSMANAGER_H__

#include "common/types.h"
#include "multitasking/process.h"
#include "hwcomm/interrupts.h"


namespace microkernel
{
    class ProcessManager : public InterruptHandler
    {
    friend class Kernel;
    public:
        ProcessManager(InterruptManager* manager, GlobalDescriptorTable* gdt);
        ~ProcessManager();

        /**
         * @brief Add the given task to the list of tasks.
         * 
         * @param process The task to be added.
         * @return The PID of the task if added, -1 if not.
         */
        uint8_t add_proc(Process* process);

        /**
         * @brief Do Round Robin scheduling to pick the next
         * process to run
         * 
         * @param cpu_state 
         * @return CPUState* 
         */
        CPUState* schedule(CPUState* cpu_state);

        void exit_proc(uint8_t pid);

        virtual uint32_t handle_interrupt(uint32_t esp);

        void print_tasks();

        void dump_stack();

    private:
        Process* tasks[256];
        uint16_t num_of_tasks;
        uint16_t current_task;
        MemManager process_mem_manager;
        MemManager stack_manager;

        Process* get_current_task();
        Process* get_process_mem();
        // uint8_t fork(
        //     uint8_t pid, 
        //     GlobalDescriptorTable* t_gdt, 
        //     uint8_t* t_stack_p, 
        //     void* t_entry_point,
        //     CPUState* cpu
        // );
    };
    
} // namespace microkernel


#endif