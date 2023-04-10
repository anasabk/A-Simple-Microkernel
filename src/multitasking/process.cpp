#include "multitasking/process.h"


using namespace microkernel;


Process::Process(
    GlobalDescriptorTable* t_gdt, 
    void* t_entry_point, 
    uint8_t* t_stack_p,
    uint32_t t_stack_size,
    Process* t_parent) 
{
    setup(
        t_gdt, 
        t_entry_point, 
        t_stack_p,
        t_stack_size,
        t_parent
    );

    num_of_children = 0;
    pid = -1;
    child = nullptr;
}

Process::~Process()
{
}

void Process::setup(
    GlobalDescriptorTable* t_gdt, 
    void* t_entry_point, 
    uint8_t* t_stack_p,
    uint32_t t_stack_size,
    Process* t_parent) 
{
    state = READY;
    stack_size = t_stack_size;

    // parent = t_parent;
    // sibling = parent->child;
    // parent->child = this;
    // parent->num_of_children++;

    stack = t_stack_p;
    cpu_state = (CPUState*)(stack - sizeof(CPUState));

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
    cpu_state->esp = 0;
    
    cpu_state->eip = (uint32_t)t_entry_point;
    cpu_state->eflags = 0x202;
}

void Process::fork(
    GlobalDescriptorTable* t_gdt,
    uint8_t* t_stack_p,
    void* t_return_addr,
    CPUState* t_cpu_state,
    Process* t_parent) 
{
    state = READY;
    stack_size = t_parent->stack_size;
    
    parent = t_parent;
    sibling = parent->child;
    parent->child = this;
    parent->num_of_children++;

    stack = t_stack_p + 12;
    uint32_t used_stack_size = (uint32_t)t_parent->stack - (uint32_t)t_cpu_state - 16;
    cpu_state = (CPUState*)((uint32_t)stack - used_stack_size);

    for(int i = used_stack_size; i > 0; i--){
        *(stack - i) = *(t_parent->stack - i);
        // printfHex(*(t_parent->stack - i));
        // printf(" ");
    }
    // printf("\n");

    *cpu_state = *t_cpu_state;

    cpu_state->cs = t_gdt->get_css();
    cpu_state->ds = t_gdt->get_dss();
    cpu_state->ss = t_gdt->get_sss();

    cpu_state->ebp = (uint32_t)t_stack_p;
    cpu_state->esp = 0;

    cpu_state->eip = (uint32_t)t_return_addr;
    cpu_state->eax = 0;

    // for(int i = used_stack_size; i > 0; i--){
        // printfHex(*(stack - i));
        // printf(" ");
    // }
    // printf("\n");

    // dump_cpu(t_cpu_state);
}

CPUState* Process::execve(
    CPUState* t_cpu_state,
    GlobalDescriptorTable* t_gdt,
    void** param,
    void* t_entry_point)
{
    // stack -= 12;
    cpu_state = (CPUState*)(stack - sizeof(CPUState));

    cpu_state->cs = t_gdt->get_css();
    cpu_state->ds = t_gdt->get_dss();
    cpu_state->ss = ((uint32_t)param);

    cpu_state->eax = 0;
    cpu_state->ebx = 0;
    cpu_state->ecx = 0;
    cpu_state->edx = 0;

    cpu_state->esi = 0;
    cpu_state->edi = 0;
    cpu_state->ebp = (uint32_t)stack;
    cpu_state->esp = 0;
    
    cpu_state->eip = (uint32_t)t_entry_point;
    cpu_state->eflags = 0x202;

    // *t_cpu_state = *cpu_state;

    // *((uint32_t*)stack - 4) = ((uint32_t)param);

#ifdef DEBUG_FLAG
    printfHex32((uint32_t)&(cpu_state->ss));
    printf(" ");
    printf("execve ");
    dump_cpu(cpu_state);
#endif

    return cpu_state;
}

uint32_t Process::get_stack_size() {
    return stack_size;
}

uint8_t Process::get_pid() {
    return pid;
}

void microkernel::dump_cpu(CPUState* cpu_state) {
    printf("cpu state: 0x");
    printfHex32(cpu_state->cs);
    printf(" 0x");
    printfHex32(cpu_state->ss);
    printf(" 0x");
    printfHex32(cpu_state->ds);
    // printf(" 0x");
    // printfHex32(cpu_state->es);
    // printf(" 0x");
    // printfHex32(cpu_state->fs);
    // printf(" 0x");
    // printfHex32(cpu_state->gs);
    printf(" 0x");
    printfHex32(cpu_state->eax);
    printf(" 0x");
    printfHex32(cpu_state->ebx);
    printf(" 0x");
    printfHex32(cpu_state->ecx);
    printf(" 0x");
    printfHex32(cpu_state->edx);
    printf(" 0x");
    printfHex32(cpu_state->ebp);
    printf(" 0x");
    printfHex32(cpu_state->esp);
    printf(" 0x");
    printfHex32(cpu_state->esi);
    printf(" 0x");
    printfHex32(cpu_state->edi);
    printf(" 0x");
    printfHex32(cpu_state->eip);
    printf("\n");
}