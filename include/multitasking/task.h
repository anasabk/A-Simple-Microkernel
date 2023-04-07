#ifndef __TASK_H__
#define __TASK_H__

#include "common/types.h"
#include "gdt.h"
#include "memory/memmanager.h"


namespace microkernel
{
    enum State {
        RUNNING,
        READY,
        BLOCKED
    };

    class Taskmanager;

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
        uint32_t gs;
        uint32_t fs;
        uint32_t es;
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

    class Task
    {
    friend class Taskmanager;

    public:
        Task();
        
        /**
         * @brief Construct a new Task object.
         * 
         * @param t_gdt The Global Descriptor Table.
         * @param t_entry_point Pointer to the instruction to start from.
         * @param t_stack_p Pointer to the stack of the stask. 
         * (Should be in the stack segment of the given gdt)
         * @param t_parent The of this task. 
         */
        Task(
            GlobalDescriptorTable* t_gdt, 
            void t_entry_point(), 
            uint8_t* t_stack_p, 
            Task* t_parent
        );
        
        ~Task();

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
            Task* t_parent
        );

        void copy(
            GlobalDescriptorTable* t_gdt,
            uint8_t* t_stack_p,
            CPUState* t_cpu_state,
            Task* t_parent
        );

        void fork(
            GlobalDescriptorTable* t_gdt,
            uint8_t* t_stack_p,
            void* t_return_addr,
            CPUState* t_cpu_state,
            Task* t_parent
        );

        void vfork(
            GlobalDescriptorTable* t_gdt,
            uint8_t* t_stack_p,
            void* t_return_addr,
            CPUState* t_cpu_state,
            Task* t_parent
        );

        void execve(
        GlobalDescriptorTable* t_gdt,
            void* t_entry_point
        );

        uint32_t get_stack_size();

        uint8_t get_pid();

    private:
        CPUState* cpu_state;
        uint8_t* stack;
        uint32_t stack_size;
        uint8_t pid;
        Task* parent;
        State state;
        Task* children[256];
        uint16_t num_of_children;

    };

    void dump_cpu(CPUState* state);
    
} // namespace microkernel


#endif
