#ifndef __TASK_H__
#define __TASK_H__

#include "common/types.h"
#include "gdt.h"
#include "memory/memmanager.h"


namespace microkernel
{
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

    enum ProcessState {
        RUNNING,
        READY,
        BLOCKED,
        TERMINATED
    };

    struct Message {
        ProcessState state;
        uint8_t pid;
        Message* next;
    };

    class Process
    {
    friend class ProcessManager;
    public:
        /**
         * @brief Construct a new Task object.
         * 
         * @param t_gdt Pointer to the global descriptor table.
         * @param t_entry_point Pointer to the first instruction in execution.
         * @param t_stack_p Pointer to the stack. 
         * (Should be in the stack segment of the given gdt)
         * @param t_parent Pointer to the parent process entry.
         */
        Process(
            GlobalDescriptorTable* t_gdt, 
            void(* t_entry_point)(), 
            uint8_t* t_stack_p,
            uint32_t t_stack_size,
            Process* t_parent
        );
        
        ~Process();

        /**
         * @brief Modify the the info of a task.
         * 
         * @param t_gdt Pointer to the global descriptor table.
         * @param t_entry_point Pointer to the first instruction in execution.
         * @param t_stack_p Pointer to the stack. 
         * (Should be in the stack segment of the given gdt)
         * @param stack_size Size of the stack.
         * @param t_parent Pointer to the parent process entry.
         */
        void setup(
            GlobalDescriptorTable* t_gdt, 
            void(* t_entry_point)(), 
            uint8_t* t_stack_p,
            uint32_t stack_size
        );

        /**
         * @brief Get the stack size.
         */
        uint32_t get_stack_size();

        /**
         * @brief Get the pid of the process.
         */
        uint8_t get_pid();

        /**
         * @brief Send msg to the parent.
         * 
         * @param t_msg The message to be sent.
         */
        void sig_to_parent(Message* t_msg);

        /**
         * @brief Search for the message with the given state 
         * and pid and return a pointer to it if found. If not, 
         * return nullptr.
         * 
         * @param state State of the message.
         * @param pid PID of the sender of the message.
         * @return Pointer to the message, or nullptr.
         */
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
        Message* msg_queue_head;
        Message* msg_queue_tail;

    };
    
} // namespace microkernel


#endif
