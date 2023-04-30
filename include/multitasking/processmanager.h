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
        ProcessManager(InterruptManager* t_manager, GlobalDescriptorTable* t_gdt);
        ~ProcessManager();

        /**
         * @brief Add the given process to the list of processes.
         * 
         * @param t_process The process to be added.
         * @return The PID of the process if added, -1 if not.
         */
        uint8_t add_proc(Process* t_process);

        /**
         * @brief Do Round Robin scheduling to pick the next
         * process to run
         * 
         * @param t_cpu_state 
         * @return CPUState* 
         */
        CPUState* schedule(CPUState* t_cpu_state);

        /**
         * @brief Handle timer Interrupts.
         * 
         * @param t_esp Current CPU state.
         * @return New CPU state.
         */
        virtual uint32_t handle_interrupt(uint32_t t_esp);

        /**
         * @brief Dump the processes in the process table.
         */
        void print_processes();

        /**
         * @brief Fork a new process from t_parent, which will continue
         * execution from t_return_addr, and has the stack pointed by
         * t_stack_p.
         * 
         * @param t_gdt The global descriptor table.
         * @param t_stack_p The new stack pointer.
         * @param t_return_addr The address where the child will resume
         * execution.
         * @param t_cpu_state The currnt cpu state.
         * @param t_parent The parent process to fork from.
         * @return The PID of the new child process.
         */
        uint8_t fork_proc(
            GlobalDescriptorTable* t_gdt,
            uint8_t* t_stack_p,
            void(* t_return_addr)(),
            CPUState* t_cpu_state,
            Process* t_parent
        );

        /**
         * @brief Change the execution of the given process to 
         * the t_entry_point, with the parameters t_parameters.
         * 
         * @param t_gdt The global descriptor table.
         * @param t_process The process in question.
         * @param t_parameters The parameters of the new function.
         * @param t_entry_point Pointer to the new function to execute.
         * @return The new CPU state of the process.
         */
        CPUState* execve_proc(
            GlobalDescriptorTable* t_gdt,
            Process* t_process,
            void** t_parameters,
            void(* t_entry_point)(void*)
        );

        /**
         * @brief Terminate the process with the PID t_pid.
         * 
         * @param t_pid The PID of the process to be terminated.
         * @return Pointer to the stack of the process if terminated, 
         * or nullptr if failed. 
         */
        uint8_t* term_proc(uint8_t t_pid);

    private:
        Process* processes[256];
        uint16_t num_of_processes;
        int16_t current_index;
        uint32_t last_pid;
        MemManager process_mem_manager;

        Process* get_current_process();

        Process* get_process_mem();
    };
    
} // namespace microkernel


#endif