#ifndef __TASKMANAGER_H__
#define __TASKMANAGER_H__


#include "common/types.h"
#include "multitasking/task.h"
#include "hwcomm/interrupts.h"


namespace microkernel
{
    class Taskmanager : public InterruptHandler
    {
    public:
        Taskmanager(InterruptManager* manager);
        ~Taskmanager();

        uint8_t add_task(Task* task);
        CPUState* schedule(CPUState* cpu_state);

        virtual uint32_t handle_interrupt(uint32_t esp);

    private:
        Task* tasks[256];
        uint16_t num_of_tasks;
        uint16_t current_task;
    
    };
    
} // namespace microkernel


#endif