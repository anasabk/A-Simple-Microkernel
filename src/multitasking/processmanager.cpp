#include "multitasking/processmanager.h"

using namespace microkernel;

ProcessManager::ProcessManager(InterruptManager* t_manager, GlobalDescriptorTable* t_gdt)
    : InterruptHandler(t_manager, 0x20),
      process_mem_manager(t_gdt->get_pss())
{
    num_of_processes = 0;
    current_index = -1;
    last_pid = -1;

    // Initialize the process table.
    for(int i = 0; i < 256; i++)
        processes[i] = nullptr;
}

ProcessManager::~ProcessManager()
{
}

uint8_t ProcessManager::add_proc(Process* t_process) {
    if(num_of_processes >= 256)
        return -1;

    // Search fo ran empty space in the table.
    int16_t index;
    for (index = 0; index < 256; index++)
        if(processes[index] == nullptr)
            break;

    // Update the data.
    processes[index] = t_process;
    last_pid++;
    t_process->pid = last_pid;
    t_process->state = READY;
    num_of_processes++;

    return t_process->pid;
}

uint32_t ProcessManager::handle_interrupt(uint32_t esp) {
    // return (uint32_t)schedule((CPUState*)esp);
    return esp;
}

CPUState* ProcessManager::schedule(CPUState* t_cpu_state) {
    if(num_of_processes <= 0)
        return t_cpu_state;

    // Check if there was a process in execution when this was called.
    if(current_index >= 0) {
        processes[current_index]->cpu_state = t_cpu_state;
        processes[current_index]->state = READY;
    }

    // Do Round Robin scheduling to find a process that is ready.
    do {
        current_index++;
        if(current_index >= 256)
            current_index %= 256;
    } while (
        processes[current_index] == nullptr ||
        processes[current_index]->state != READY);
    
    // Change the currently executing process.
    processes[current_index]->state = RUNNING;
    print_processes();
    return processes[current_index]->cpu_state;
}

void ProcessManager::print_processes() {
    printf("process table:\n");
    for(int i = 0; i < 255; i++) {
        if(processes[i] != nullptr) {
            printf("process pid: ");
            printf_int(processes[i]->pid);
            printf(", state: ");
            switch (processes[i]->state)
            {
            case READY:
                printf("READY");
                break;

            case RUNNING:
                printf("RUNNING");
                break;

            case BLOCKED:
                printf("BLOCKED");
                break;
            
            default:
                printf("UNKNOWN");
                break;
            }
            printf("\n");
        }
    }
}

uint8_t* ProcessManager::term_proc(uint8_t t_pid) {
    if(t_pid < 0 || t_pid > 255)
        return nullptr; 

    // Search for the process.
    Process* target = nullptr;
    int16_t index;
    for (index = 0; index < 256; index++)
        if(processes[index] != nullptr && processes[index]->pid == t_pid){
            target = processes[index];
            break;
        }
    
    // The process was not found.
    if(target == nullptr)
        return nullptr;

    // Add the process's children to the init process's children.
    Process* child = target->child;
    Process* temp = child;
    while(child != nullptr) {
        temp = child->sibling;
        child->sibling = processes[0]->child;
        processes[0]->child = child;
        child = temp;
        processes[0]->num_of_children++;
    }

    // Remove the process form its parent's children list.
    if(target->parent->child == target)
        target->parent->child = target->sibling;

    for(Process* sibling = target->parent->child, *prev = nullptr;
        sibling != nullptr;
        prev = sibling, sibling = sibling->sibling)
    {
        if(sibling == target) {
            prev->sibling = target->sibling;
            break;
        }
    }

    // Send a message to the parent specifying the termination.
    Message* msg = (Message*)process_mem_manager.get_chunk(sizeof(Message));
    msg->next = nullptr;
    msg->state = TERMINATED;
    msg->pid = processes[index]->pid;
    target->sig_to_parent(msg);

    // Free the process's entry memory.
    uint8_t* temp_stack = target->stack;
    process_mem_manager.free(target);

    // Update the process table.
    processes[index] = nullptr;
    num_of_processes--;
    
    // Return the stack pointer of the process.
    return temp_stack;
}

uint8_t ProcessManager::fork_proc(
    GlobalDescriptorTable* t_gdt,
    uint8_t* t_stack_p,
    void(* t_return_addr)(),
    CPUState* t_cpu_state,
    Process* t_parent) 
{
    // Update the parent's cpu state pointer.
    t_parent->cpu_state = t_cpu_state;

    // Create the new task entry.
    Process* new_task = get_process_mem();
    new_task->parent = t_parent;
    new_task->state = READY;
    new_task->sibling = new_task->parent->child;
    new_task->parent->child = new_task;
    new_task->parent->num_of_children++;

    // Organize the new process's stack
    new_task->stack_size = t_parent->stack_size;
    new_task->stack = t_stack_p + 12;
    uint32_t used_stack_size = (uint32_t)t_parent->stack - (uint32_t)t_cpu_state - 16;
    new_task->cpu_state = (CPUState*)((uint32_t)new_task->stack - used_stack_size);

    for(int i = used_stack_size; i > 0; i--)
        *(new_task->stack - i) = *(t_parent->stack - i);

    // Copy the CPU state form the parent to the child.
    *new_task->cpu_state = *t_cpu_state;

    new_task->cpu_state->cs = t_gdt->get_css();
    new_task->cpu_state->ds = t_gdt->get_dss();
    new_task->cpu_state->ss = t_gdt->get_sss();

    new_task->cpu_state->ebp = (uint32_t)t_stack_p;
    new_task->cpu_state->esp = 0;

    // Set the return address of the child.
    new_task->cpu_state->eip = (uint32_t)t_return_addr;

    // Set the return value of execve to 0 for the child.
    new_task->cpu_state->eax = 0;

    // Add the child to the process table.
    add_proc(new_task);

    return new_task->get_pid();
}

CPUState* ProcessManager::execve_proc(
    GlobalDescriptorTable* t_gdt,
    Process* t_process,
    void** t_parameters,
    void(* t_entry_point)(void*)) 
{
    Process* current_task = get_current_process();

    // Set the location of the new cpu state, which is immediately after the stack base.
    t_process->cpu_state = (CPUState*)(t_process->stack - sizeof(CPUState));

    t_process->cpu_state->cs = t_gdt->get_css();
    t_process->cpu_state->ds = t_gdt->get_dss();

    // Place the pointer to the parameters.
    t_process->cpu_state->ss = (uint32_t)t_parameters;

    t_process->cpu_state->eax = 0;
    t_process->cpu_state->ebx = 0;
    t_process->cpu_state->ecx = 0;
    t_process->cpu_state->edx = 0;

    t_process->cpu_state->esi = 0;
    t_process->cpu_state->edi = 0;
    t_process->cpu_state->ebp = (uint32_t)t_process->stack;
    t_process->cpu_state->esp = 0;
    
    t_process->cpu_state->eip = (uint32_t)t_entry_point;
    t_process->cpu_state->eflags = 0x202;

    return t_process->cpu_state;
}

Process* ProcessManager::get_current_process() {
    return processes[current_index];
}

Process* ProcessManager::get_process_mem() {
    return (Process*)process_mem_manager.get_chunk(sizeof(Process));
}
