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

    uint32_t result = last_stack_p - sizeof(StackChunk);
    ((StackChunk*)result)->is_free = false;
    ((StackChunk*)result)->next_chunk = nullptr;
    ((StackChunk*)result)->size = t_size;

    last_stack_p = result - t_size;
    
    return (void*)result;
}

void MemManager::free(void* t_stack_p) {
    if(((StackChunk*)t_stack_p)->is_free)
        return;

    if((uint32_t)t_stack_p - ((StackChunk*)t_stack_p)->size == last_stack_p) {
        last_stack_p = (uint32_t)t_stack_p + sizeof(StackChunk);
        return;
    }

    StackChunk *temp, *prev;
    for(temp = free_list, prev = nullptr; 
        temp != nullptr; 
        prev = temp, temp = temp->next_chunk) 
    {
        if(t_stack_p > temp)
            break;
    }

    if(prev == nullptr) {
        ((StackChunk*)t_stack_p)->next_chunk = free_list;
        free_list = ((StackChunk*)t_stack_p);

    } else {
        prev->next_chunk = ((StackChunk*)t_stack_p);

        // if((uint32_t)t_stack_p - target->size == temp)
        //     target->next_chunk = temp->next_chunk;
        // else
            ((StackChunk*)t_stack_p)->next_chunk = temp;
    }
}
