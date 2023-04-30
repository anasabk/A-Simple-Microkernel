#include "kernel.h"
#include "common/tests.h"

using namespace microkernel;

Kernel* Kernel::active_kernel;

Kernel::Kernel()
    : InterruptHandler(interrupt_manager, SYSCALL_INT_NUM)
{
}

Kernel::~Kernel()
{
}


uint16_t Kernel::get_current_pid() 
{
    return active_kernel->process_manager->get_current_process()->get_pid();
}

uint32_t Kernel::handle_interrupt(uint32_t esp) 
{
    CPUState* cpu = (CPUState*)esp;

    switch (cpu->eax)
    {
    case 1:
        cpu = sys_exit(cpu);
        break;

    case 2:
        sys_fork(cpu);
        break;

    case 7: 
        sys_waitpid(cpu);
        break;

    case 11:
        cpu = sys_execve(cpu);
        break;

    default:
        break;
    }

    return (uint32_t)cpu;
}

void Kernel::run()
{
    GlobalDescriptorTable l_gdt;
    gdt = &l_gdt;

    InterruptManager l_interrupt_manager(HARDWARE_INT_OFFSET, gdt);
    interrupt_manager = &l_interrupt_manager;

    ProcessManager l_process_manager(interrupt_manager, gdt);
    process_manager = &l_process_manager;

    MemManager stack_manager(gdt->get_sss());
    ss_manager = &stack_manager;

    KeyboardDriver l_keyboard_driver(interrupt_manager);
    keyboard_driver = &l_keyboard_driver;

    MouseDriver l_mouse_driver(interrupt_manager);
    mouse_driver = &l_mouse_driver;

    set_manager(interrupt_manager);

    active_kernel = this;

    interrupt_manager->activate();

    in_buffer[0] = 0;
    buf_index = -1;
    ready = -1;

    // Create the init process.
    Process* new_task = process_manager->get_process_mem();
    
    uint8_t* new_stack = 
        (uint8_t*)ss_manager->get_chunk(4096) + 4096 - 1;

    new_task->setup(
        gdt,
        init,
        new_stack,
        16384
    );

    process_manager->add_proc(new_task);

    while(1);
}


void Kernel::sys_fork(CPUState* t_cpu_state) {
    Process* current_task = process_manager->get_current_process();
    uint8_t* new_stack = 
        (uint8_t*)ss_manager->get_chunk(current_task->get_stack_size()) + 
        current_task->get_stack_size() - 1;
    
    t_cpu_state->ebx = process_manager->fork_proc(
        gdt,
        new_stack,
        (void(*)())t_cpu_state->ecx,
        t_cpu_state,
        current_task
    );
}

void Kernel::sys_waitpid(CPUState* t_cpu_state) {
    int16_t pid = (int16_t)t_cpu_state->ebx;
    ProcessState state = (ProcessState)t_cpu_state->ecx;
    Process* current_task = 
        process_manager->get_current_process();

    Message* msg = current_task->rec_msg(state, pid);
    if(msg != nullptr){
        int16_t target = msg->pid;
        process_manager->process_mem_manager.free(msg);
        t_cpu_state->edx = target;
    } else {
        t_cpu_state->edx = -1;
    }
}

CPUState* Kernel::sys_exit(CPUState* t_cpu_state) {
    uint8_t* old_stack = process_manager->term_proc(get_current_pid());
    ss_manager->free(old_stack);
    process_manager->current_index = -1;
    return process_manager->schedule(t_cpu_state);
}

CPUState* Kernel::sys_execve(CPUState* t_cpu_state) {
    return process_manager->execve_proc(
        gdt,
        process_manager->get_current_process(),
        (void**)t_cpu_state->ecx,
        (void(*)(void*))t_cpu_state->ebx 
    );
}


void Kernel::get_processes() {
    active_kernel->process_manager->print_processes();
}


uint16_t Kernel::fork()
{
    uint32_t result_child;
    asm("int $0x80" : "=b" (result_child) : "a" (2), "c" (__builtin_return_address(0)));
    return result_child;
}

void Kernel::exit() {
    asm("int $0x80" : : "a" (1));
}

int16_t Kernel::waitpid(uint16_t pid, ProcessState status) {
    int16_t result;
    asm("int $0x80" : "=d" (result) : "a" (7), "b" (pid), "c" (status));
    return result;
}

int Kernel::execve(void (*t_func_pointer)(void*), void* t_argv) {
    asm("int $0x80" : : "a" (11), "b" (t_func_pointer), "c" (t_argv));
    return -1;
}


uint32_t Kernel::schedule(uint32_t esp) {
    return (uint32_t) active_kernel->process_manager->schedule((CPUState*) esp);
}
