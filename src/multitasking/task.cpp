#include "multitasking/task.h"


using namespace microkernel;

Task::Task(
    GlobalDescriptorTable* t_gdt, 
    void t_entry_point(), 
    uint32_t t_stack_size, 
    void* t_stack_p)
{
    stack = t_stack_p - sizeof(CPUState);
    cpu_state = (CPUState*)t_stack_p;

    cpu_state->cs = t_gdt->get_css();
    cpu_state->ds = t_gdt->get_dss();
    cpu_state->ss = t_gdt->get_sss();

    cpu_state->eax = 0;
    cpu_state->ebx = 0;
    cpu_state->ecx = 0;
    cpu_state->edx = 0;

    cpu_state->esi = 0;
    cpu_state->edi = 0;
    cpu_state->ebp = 0;
    
    cpu_state->eip = (uint32_t)t_entry_point;
    cpu_state->eflags = 0x202;
}

Task::~Task()
{
}
