#include "multitasking/taskmanager.h"

using namespace microkernel;
    
Taskmanager::Taskmanager(InterruptManager* t_manager)
    : InterruptHandler(t_manager, 0x20)
{
    num_of_tasks = 0;
    current_task = -1;
}

Taskmanager::~Taskmanager()
{
}

uint8_t Taskmanager::add_task(Task* t_task) {
    if(num_of_tasks >= 256)
        return -1;

    tasks[num_of_tasks] = t_task;
    t_task->pid = num_of_tasks;
    num_of_tasks++;

    return 0;
}

uint32_t Taskmanager::handle_interrupt(uint32_t esp) {
    return (uint32_t)schedule((CPUState*)esp);
}

CPUState* Taskmanager::schedule(CPUState* t_cpu_state) {
    if(num_of_tasks <= 0)
        return t_cpu_state;

    if(current_task >= 0) 
        tasks[current_task]->cpu_state = t_cpu_state;

    current_task++;
    if(current_task >= num_of_tasks)
        current_task %= num_of_tasks;

    return tasks[current_task]->cpu_state;
}
