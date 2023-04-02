#include "multitasking/task.h"


using namespace microkernel;

Task::Task(
    GlobalDescriptorTable* t_gdt, 
    void t_entry_point(), 
    uint32_t t_stack_size, 
    MemManager* t_mem_manager)
{
    stack = t_mem_manager->get_stack_chunk(t_stack_size + sizeof(CPUState)) - sizeof(CPUState);
    printf("Stack start: ");
    printfHex32((uint32_t)stack);
    printf("\n");
    cpu_state = (CPUState*)stack;

    cpu_state->cs = t_gdt->get_css();
    cpu_state->ds = t_gdt->get_dss();
    cpu_state->ss = t_gdt->get_sss();

    cpu_state->eax = 0;
    cpu_state->ebx = 0;
    cpu_state->ecx = 0;
    cpu_state->edx = 0;

    cpu_state->esi = 0;
    cpu_state->edi = 0;
    cpu_state->ebp = (uint32_t)stack;
    cpu_state->esp = (uint32_t)stack;
    
    cpu_state->eip = (uint32_t)t_entry_point;
    cpu_state->eflags = 0x202;
}

Task::~Task()
{
}
