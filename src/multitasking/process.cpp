#include "multitasking/process.h"


using namespace microkernel;


Process::Process(
    GlobalDescriptorTable* t_gdt, 
    void(* t_entry_point)(), 
    uint8_t* t_stack_p,
    uint32_t t_stack_size,
    Process* t_parent) 
{
    setup(
        t_gdt, 
        t_entry_point, 
        t_stack_p,
        t_stack_size
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
    void(* t_entry_point)(), 
    uint8_t* t_stack_p,
    uint32_t t_stack_size) 
{
    state = READY;
    stack_size = t_stack_size;
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

uint32_t Process::get_stack_size() {
    return stack_size;
}

uint8_t Process::get_pid() {
    return pid;
}

void Process::sig_to_parent(Message* t_msg) {
    if(parent->msg_queue_head == nullptr) {
        parent->msg_queue_head = t_msg;
        parent->msg_queue_tail = t_msg;
    } else {
        // Add the message to the end of the message list of the parent.
        msg_queue_tail->next = t_msg;
        msg_queue_tail = t_msg;
    }
}

Message* Process::rec_msg(ProcessState state, int16_t pid) {
    // Iterate through the message list.
    Message* prev = nullptr;
    for(Message* msg = msg_queue_head;
        msg != nullptr;
        prev = msg, msg = msg->next)
    {
        // Check if the message matches the target.
        if (msg->state == state && 
            (pid == -1 || msg->pid == pid)) 
        {
            if(msg == msg_queue_head)
                msg_queue_head = msg->next;
            else
                prev->next = msg->next;
            return msg;
        }
    }

    return nullptr;
}
