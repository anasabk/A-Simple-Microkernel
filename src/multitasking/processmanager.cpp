#include "multitasking/processmanager.h"

using namespace microkernel;

ProcessManager::ProcessManager(InterruptManager* t_manager, GlobalDescriptorTable* gdt)
    : InterruptHandler(t_manager, 0x20),
      process_mem_manager(gdt->get_tss()),
      stack_manager(gdt->get_sss())
{
    num_of_tasks = 0;
    current_task = -1;
}

ProcessManager::~ProcessManager()
{
}

uint8_t ProcessManager::add_proc(Process* t_task) {
    if(num_of_tasks >= 256)
        return -1;

    tasks[num_of_tasks] = t_task;
    t_task->pid = num_of_tasks;
    t_task->state = READY;
    num_of_tasks++;

    return t_task->pid;
}

uint32_t ProcessManager::handle_interrupt(uint32_t esp) {
    return (uint32_t)schedule((CPUState*)esp);
}

CPUState* ProcessManager::schedule(CPUState* t_cpu_state) {
    // printf("scheduling\n");
    if(num_of_tasks <= 0)
        return t_cpu_state;

    if(current_task >= 0) {
        tasks[current_task]->cpu_state = t_cpu_state;
        tasks[current_task]->state = READY;
    }

    do {
        current_task++;
        if(current_task >= num_of_tasks)
            current_task %= num_of_tasks;
    } while (tasks[current_task]->state != READY);

#ifdef DEGBUG_FLAG
    printf("new ");
    // microkernel::dump_cpu(tasks[current_task]->cpu_state);
    printf("stack: 0x");
    printfHex32((uint32_t)tasks[current_task]->stack);
    printf(" 0x");
    printfHex32((uint32_t)tasks[current_task]->cpu_state);
    printf("\n");
#endif
    
    tasks[current_task]->state = RUNNING;
    return tasks[current_task]->cpu_state;
}

void ProcessManager::print_tasks() {
    printf("\n");
    for(int i = 0; i < num_of_tasks; i++) {
        printf("task ");
        printfHex32(tasks[i]->pid);
        printf(": ");
        printfHex32((uint32_t)tasks[i]->stack);
        printf("\n");
    }
    printf("\n");
}

void ProcessManager::dump_stack() {
    Process* current = get_current_task();

    for(int i = 1; i < 116; i++){
        printfHex(*(current->stack - i));
        printf(" ");
    }

    printf("\n");
}

void ProcessManager::exit_proc(uint8_t pid) {
    if(pid < 0 || pid >= num_of_tasks)
        return; 

    Process* target = tasks[pid];
    target->state = TERMINATED;

}

Process* ProcessManager::get_current_task() {
    return tasks[current_task];
}

Process* ProcessManager::get_process_mem() {
    return (Process*)process_mem_manager.get_chunk(sizeof(Process));
}
