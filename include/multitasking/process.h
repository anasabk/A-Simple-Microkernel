#ifndef __TASK_H__
#define __TASK_H__

#include "common/types.h"
#include "gdt.h"
#include "memory/memmanager.h"


namespace microkernel
{
    enum ProcessState {
        RUNNING,
        READY,
        BLOCKED,
        TERMINATED
    };
    
    class ProcessManager;

    struct CPUState
    {
        // General registers
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;

        uint32_t esi;
        uint32_t edi;
        uint32_t ebp;

        // Segment Registers
        uint32_t ds;

        // Error Code For Exceptions
        uint32_t error;

        // Registers Dumped by the Interrupt
        uint32_t eip;
        uint32_t cs;
        uint32_t eflags;
        uint32_t esp;
        uint32_t ss;
    } __attribute__((packed));

    struct Message {
        ProcessState state;
        uint8_t pid;
        Message* next;
    };

    class Process
    {
    friend class ProcessManager;

    public:
        // Process();
        
        /**
         * @brief Construct a new Task object.
         * 
         * @param t_gdt The Global Descriptor Table.
         * @param t_entry_point Pointer to the instruction to start from.
         * @param t_stack_p Pointer to the stack of the stask. 
         * (Should be in the stack segment of the given gdt)
         * @param t_parent The of this task. 
         */
        Process(
            GlobalDescriptorTable* t_gdt, 
            void* t_entry_point, 
            uint8_t* t_stack_p,
            uint32_t t_stack_size,
            Process* t_parent
        );
        
        ~Process();

        /**
         * @brief Modify the the info of a task.
         * 
         * @param t_gdt 
         * @param t_entry_point 
         * @param t_stack_p 
         * @param stack_size 
         * @param t_parent 
         */
        void setup(
            GlobalDescriptorTable* t_gdt, 
            void* t_entry_point, 
            uint8_t* t_stack_p,
            uint32_t stack_size,
            Process* t_parent
        );

        void fork(
            GlobalDescriptorTable* t_gdt,
            uint8_t* t_stack_p,
            void* t_return_addr,
            CPUState* t_cpu_state,
            Process* t_parent
        );

        CPUState* execve(
            CPUState* t_cpu_state,
            GlobalDescriptorTable* t_gdt,
            void** param,
            void* t_entry_point
        );

        // uint8_t* terminate();

        uint32_t get_stack_size();

        uint8_t get_pid();

        void sigchld(Message* t_msg);

        Message* rec_msg(ProcessState state, int16_t pid);

    private:
        CPUState* cpu_state;
        uint8_t* stack;
        uint32_t stack_size;
        uint8_t pid;
        Process* parent;
        ProcessState state;
        Process* sibling;
        Process* child;
        uint8_t num_of_children;
        Message* msg_queue;

    };

    void dump_cpu(CPUState* state);
    
} // namespace microkernel


#endif
