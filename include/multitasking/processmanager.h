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
         * @brief Add the given process to the list of processes.
         * 
         * @param process The process to be added.
         * @return The PID of the process if added, -1 if not.
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

        uint8_t* exit_proc(uint8_t pid);

        virtual uint32_t handle_interrupt(uint32_t esp);

        void print_processes();

        void dump_stack();

    private:
        Process* processes[256];
        uint16_t num_of_processes;
        int16_t current_index;
        MemManager process_mem_manager;

        Process* get_current_process();
        Process* get_process_mem();
    };
    
} // namespace microkernel


#endif