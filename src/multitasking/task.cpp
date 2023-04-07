#include "multitasking/task.h"


using namespace microkernel;


Task::Task(
    GlobalDescriptorTable* t_gdt, 
    void t_entry_point(), 
    uint8_t* t_stack_p, 
    Task* t_parent)
{
    state = READY;
    parent = t_parent;
    stack_size = t_parent->stack_size;
    t_parent->children[t_parent->num_of_children] = this;
    t_parent->num_of_children++;

    stack = t_stack_p - sizeof(CPUState);

    for(uint32_t i = 0; i < t_parent->stack_size; i++)
        stack[i] = t_parent->stack[i];

    cpu_state = (CPUState*)stack;

    cpu_state->cs = t_gdt->get_css();
    // cpu_state->ds = t_gdt->get_dss();
    // cpu_state->ss = t_gdt->get_sss();

    cpu_state->eax = 0;
    cpu_state->ebx = 0;
    cpu_state->ecx = 0;
    cpu_state->edx = 0;

    cpu_state->esi = 0;
    cpu_state->edi = 0;
    cpu_state->ebp = 0;
    cpu_state->esp = 0;
    
    cpu_state->eip = (uint32_t)t_entry_point;
    cpu_state->eflags = 0x202;
}

Task::~Task()
{
}

void Task::setup(
    GlobalDescriptorTable* t_gdt, 
    void* t_entry_point, 
    uint8_t* t_stack_p,
    uint32_t t_stack_size,
    Task* t_parent) 
{
    state = READY;
    parent = t_parent;
    stack_size = t_stack_size;
    t_parent->children[t_parent->num_of_children] = this;
    t_parent->num_of_children++;

    // stack = t_stack_p - sizeof(CPUState);
    stack = t_stack_p;
    cpu_state = (CPUState*)(stack - sizeof(CPUState));;

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

    // dump_cpu(cpu_state);
}

void Task::copy(
    GlobalDescriptorTable* t_gdt,
    uint8_t* t_stack_p,
    CPUState* t_cpu_state,
    Task* t_parent) 
{
    state = READY;
    parent = t_parent;
    stack_size = t_parent->stack_size;
    t_parent->children[t_parent->num_of_children] = this;
    t_parent->num_of_children++;

    // stack = t_stack_p - sizeof(CPUState);
    stack = t_stack_p;
    cpu_state = (CPUState*)stack - sizeof(CPUState);

    for(int i = 0; i < stack_size; i++){
        *(stack - i) = *(t_parent->stack - i);
        // printfHex(*(stack - i));
        // printf(" ");
    }

    *cpu_state = *t_cpu_state;
    // cpu_state->cs = t_gdt->get_css();
    // cpu_state->ds = t_gdt->get_dss();
    // cpu_state->ss = ((uint32_t)stack - stack_size) >> 4;

    // cpu_state->ebp = stack_size;
    cpu_state->ebx = 0;
    // cpu_state->esp = (uint32_t)stack;
    // cpu_state->esp = (uint32_t)(stack - t_cpu_state->ebp + t_cpu_state->esp);

    // dump_cpu(cpu_state);
}

void Task::fork(
    GlobalDescriptorTable* t_gdt,
    uint8_t* t_stack_p,
    void* t_return_addr,
    CPUState* t_cpu_state,
    Task* t_parent) 
{
    // dump_cpu(t_cpu_state);
    state = READY;
    parent = t_parent;
    stack_size = t_parent->stack_size;
    t_parent->children[t_parent->num_of_children] = this;
    t_parent->num_of_children++;

    stack = t_stack_p;
    printf("current cpu state: 0x");
    printfHex32((uint32_t)t_cpu_state);
    printf(" 0x");
    printfHex32((uint32_t)t_parent->stack);
    printf(" 0x");
    printfHex32((uint32_t)stack);
    printf("\n");

    uint32_t used_stack_size = ((uint32_t)t_parent->stack - (uint32_t)t_cpu_state);
    cpu_state = (CPUState*)((uint32_t)stack - used_stack_size + 28);

    for(int i = used_stack_size; i > 0; i--)
        *(stack - i) = *(t_parent->stack - i);

    *cpu_state = *(t_cpu_state);

    cpu_state->cs = t_gdt->get_css();
    cpu_state->ds = t_gdt->get_dss();
    cpu_state->ss = (uint32_t)stack;

    cpu_state->ebp = (uint32_t)stack;
    cpu_state->esp = 0;

    cpu_state->eip = (uint32_t)t_return_addr;
    cpu_state->eax = 0;

    dump_cpu(t_cpu_state);
}

void Task::vfork(
    GlobalDescriptorTable* t_gdt,
    uint8_t* t_stack_p,
    void* t_return_addr,
    CPUState* t_cpu_state,
    Task* t_parent) 
{
    // dump_cpu(t_cpu_state);
    state = READY;
    parent = t_parent;
    stack_size = t_parent->stack_size;
    t_parent->children[t_parent->num_of_children] = this;
    t_parent->num_of_children++;

    stack = t_stack_p;
    cpu_state = (CPUState*)(stack - sizeof(CPUState));

    *cpu_state = *t_cpu_state;

    cpu_state->cs = t_gdt->get_css();
    cpu_state->ds = t_gdt->get_dss();
    cpu_state->ss = t_gdt->get_sss();

    cpu_state->ebp = (uint32_t)stack;
    cpu_state->esp = 0;

    cpu_state->eip = (uint32_t)t_return_addr;
    cpu_state->eax = 0;

    // dump_cpu(cpu_state);
}

void Task::execve(
    GlobalDescriptorTable* t_gdt,
    void* t_entry_point)
{
    // dump_cpu(t_cpu_state);
    // printf("stack: 0x");
    // printfHex32((uint32_t)stack);
    // printf("\n");
    // for(uint32_t i = 0; i < stack_size; i++)
    //     *(stack - i) = 0;
    
    cpu_state = (CPUState*)stack - sizeof(CPUState);

    cpu_state->cs = t_gdt->get_css();
    cpu_state->ds = t_gdt->get_dss();
    // cpu_state->ss = t_gdt->get_sss();

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

    dump_cpu(cpu_state);
}

uint32_t Task::get_stack_size() {
    return stack_size;
}

uint8_t Task::get_pid() {
    return pid;
}

void microkernel::dump_cpu(CPUState* cpu_state) {
    printf("cpu state: 0x");
    printfHex32(cpu_state->cs);
    printf(" 0x");
    printfHex32(cpu_state->ss);
    printf(" 0x");
    printfHex32(cpu_state->ds);
    printf(" 0x");
    printfHex32(cpu_state->es);
    printf(" 0x");
    printfHex32(cpu_state->fs);
    printf(" 0x");
    printfHex32(cpu_state->gs);
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