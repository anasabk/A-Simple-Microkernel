#include "memory/memmanager.h"


using namespace microkernel;
 
MemManager::MemManager(GlobalDescriptorTable* gdt)
{
    stack_ss = gdt->get_sss();
    last_stack_p = get_base();
    free_list = nullptr;
}

MemManager::~MemManager()
{
}

uint32_t MemManager::get_base() {
    return (stack_ss << 16) | 0x0000FFFF;
}

uint32_t MemManager::get_limit() {
    return (stack_ss << 16) | 0x00000000;
}

void* MemManager::get_stack_chunk(uint32_t t_size) {
    if(free_list != nullptr) {
        StackChunk *target, *prev;

        for(target = free_list, prev = nullptr; 
            target != nullptr; 
            prev = target, target = target->next_chunk) 
        {
            if(target->size == t_size) {
                target->is_free = false;

                if(prev == nullptr)
                    free_list = free_list->next_chunk;
                else
                    prev->next_chunk = target->next_chunk;
                
                return (uint8_t*)(target - sizeof(StackChunk));
            }
        }
    }

    if(last_stack_p - t_size - sizeof(StackChunk) < get_limit())
        return nullptr;

    ((StackChunk*)last_stack_p)->is_free = false;
    ((StackChunk*)last_stack_p)->next_chunk = nullptr;
    ((StackChunk*)last_stack_p)->size = t_size;
    uint32_t result = last_stack_p - sizeof(StackChunk);

    last_stack_p = result - t_size;
    
    return (void*)result;
}

void MemManager::free(void* t_stack_p) {
    StackChunk* target = (StackChunk*)(t_stack_p + sizeof(StackChunk));
    
    if(target->is_free)
        return;

    if((uint32_t)t_stack_p - target->size == last_stack_p) {
        last_stack_p = (uint32_t)target;
        return;
    }

    StackChunk *temp, *prev;
    for(temp = free_list, prev = nullptr; 
        temp != nullptr; 
        prev = temp, temp = temp->next_chunk) 
    {
        if(target > temp)
            break;
    }

    if(prev == nullptr) {
        target->next_chunk = free_list;
        free_list = target;

    } else {
        prev->next_chunk = target;

        // if((uint32_t)t_stack_p - target->size == temp)
        //     target->next_chunk = temp->next_chunk;
        // else
            target->next_chunk = temp;
    }
}
