#ifndef __TASK_H__
#define __TASK_H__

#include "common/types.h"
#include "gdt.h"
#include "memory/memmanager.h"


namespace microkernel
{
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

        // uint32_t gs;
        // uint32_t fs;
        // uint32_t es;
        uint32_t ds;

        uint32_t error;

        uint32_t eip;
        uint32_t cs;
        uint32_t eflags;
        uint32_t esp;
        uint32_t ss;
    };

    enum State {
        RUNNING,
        READY,
        BLOCKED
    };

    class Taskmanager;

    class Task
    {
        friend class Taskmanager;
    public:
        Task(GlobalDescriptorTable* t_gdt, void t_entry_point(), uint32_t t_stack_size, void* t_stack_p);
        ~Task();

    private:
        CPUState* cpu_state;
        void* stack;
        uint32_t pid;
        Task* parent;
        State state;
        Task* children[256];

    };
    
} // namespace microkernel


#endif
