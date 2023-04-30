#include "multitasking/processmanager.h"

using namespace microkernel;

ProcessManager::ProcessManager(InterruptManager* t_manager, GlobalDescriptorTable* gdt)
    : InterruptHandler(t_manager, 0x20),
      process_mem_manager(gdt->get_pss())
{
    num_of_processes = 0;
    current_index = -1;

    for(int i = 0; i < 256; i++)
        processes[i] = nullptr;
}

ProcessManager::~ProcessManager()
{
}

uint8_t ProcessManager::add_proc(Process* t_process) {
    if(num_of_processes >= 256)
        return -1;

    processes[num_of_processes] = t_process;
    t_process->pid = num_of_processes;
    t_process->state = READY;
    num_of_processes++;

    return t_process->pid;
}

uint32_t ProcessManager::handle_interrupt(uint32_t esp) {
    return (uint32_t)schedule((CPUState*)esp);
}

CPUState* ProcessManager::schedule(CPUState* t_cpu_state) {
    // printf("scheduling\n");
    if(num_of_processes <= 0)
        return t_cpu_state;

    if(current_index >= 0) {
        processes[current_index]->cpu_state = t_cpu_state;
        processes[current_index]->state = READY;
    }

    do {
        current_index++;
        if(current_index >= 256)
            current_index %= 256;
    } while (
        processes[current_index] == nullptr ||
        processes[current_index]->state != READY);

#ifdef DEBUG_FLAG
    printf("new ");
    // microkernel::dump_cpu(processes[current_process]->cpu_state);
    printf("stack: 0x");
    printfHex32((uint32_t)processes[current_process]->stack);
    printf(" 0x");
    printfHex32((uint32_t)processes[current_process]->cpu_state);
    printf("\n");
#endif
    
    processes[current_index]->state = RUNNING;
    return processes[current_index]->cpu_state;
}

void ProcessManager::print_processes() {
    printf("\n");
    for(int i = 0; i < num_of_processes; i++) {
        printf("process ");
        printfHex32(processes[i]->pid);
        printf(": ");
        printfHex32((uint32_t)processes[i]->stack);
        printf("\n");
    }
    printf("\n");
}

void ProcessManager::dump_stack() {
    Process* current = get_current_process();

    for(int i = 1; i < 116; i++){
        printfHex(*(current->stack - i));
        printf(" ");
    }

    printf("\n");
}

uint8_t* ProcessManager::exit_proc(uint8_t pid) {
    // printf("Exiting\n");
    if(pid < 0 || pid >= num_of_processes)
        return nullptr; 

    Process* target = nullptr;
    int16_t index = 0;

    for (size_t i = 0; i < 256; i++)
        if(processes[i]->pid == pid){
            target = processes[i];
            index = i;
            break;
        }
    
    if(target == nullptr)
        return nullptr;

    //Send children to process 0
    Process* child = target->child;
    Process* temp = child;
    while(child != nullptr) {
        temp = child->sibling;
        child->sibling = processes[0]->child;
        processes[0]->child = child;
        child = temp;
        processes[0]->num_of_children++;
    }

    //remove from children list of parent
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

    // printf("Sending a message\n");
    Message* msg = (Message*)process_mem_manager.get_chunk(sizeof(Message));
    msg->next = nullptr;
    msg->state = TERMINATED;
    msg->pid = processes[index]->pid;
    target->parent->sigchld(msg);
    // printf("Finished Sending a message\n");

    uint8_t* temp_stack = target->stack;
    process_mem_manager.free(target);

    processes[index] = nullptr;
    // num_of_processes--;
    current_index = -1;

    // for(int i = index; i < num_of_processes; i++)
    //     processes[index] = processes[pid+1];

    // printf("Exited\n");

    return temp_stack;
}

Process* ProcessManager::get_current_process() {
    return processes[current_index];
}

Process* ProcessManager::get_process_mem() {
    return (Process*)process_mem_manager.get_chunk(sizeof(Process));
}
