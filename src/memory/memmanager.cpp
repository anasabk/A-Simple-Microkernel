#include "memory/memmanager.h"


using namespace microkernel;
 
MemManager::MemManager(uint16_t t_segment_sl)
{
    segment_sl = t_segment_sl;
    last_p = get_base();
    free_list = nullptr;
}

MemManager::~MemManager()
{
}

uint32_t MemManager::get_base() {
    return (segment_sl << 16) | 0x00000000;
}

uint32_t MemManager::get_limit() {
    return (segment_sl << 16) | 0x0000FFFF;
}

void* MemManager::get_chunk(uint32_t t_size) {
    if(free_list != nullptr) {
        MemChunk *target, *prev;

        for(target = free_list, prev = nullptr; 
            target != nullptr; 
            prev = target, target = target->next_chunk) 
        {
            if(target->size == t_size) {
                if(target == free_list)
                    free_list = free_list->next_chunk;
                else
                    prev->next_chunk = target->next_chunk;
                
                target->next_chunk = nullptr;
                target->is_free = false;

                return (void*)(target + sizeof(MemChunk));
            }
        }
    }

    if(last_p + t_size + sizeof(MemChunk) > get_limit())
        return nullptr;

    ((MemChunk*)last_p)->is_free = false;
    ((MemChunk*)last_p)->next_chunk = nullptr;
    ((MemChunk*)last_p)->size = t_size;

    void* result = (void*)(last_p + sizeof(MemChunk));

    last_p = (uint32_t)(result + t_size);
    
    return result;
}

void MemManager::free(void* t_chunk_p) {
    if(((MemChunk*)t_chunk_p)->is_free)
        return;

    if((uint32_t)(t_chunk_p + ((MemChunk*)t_chunk_p)->size) == last_p) {
        last_p = (uint32_t)(t_chunk_p - sizeof(MemChunk));
        return;
    }

    MemChunk *temp, *prev;
    for(temp = free_list, prev = nullptr; 
        temp != nullptr; 
        prev = temp, temp = temp->next_chunk) 
    {
        if(t_chunk_p < temp)
            break;
    }

    if(temp == free_list) {
        ((MemChunk*)t_chunk_p)->next_chunk = free_list;
        free_list = ((MemChunk*)t_chunk_p);

    } else {
        prev->next_chunk = ((MemChunk*)t_chunk_p);
        ((MemChunk*)t_chunk_p)->next_chunk = temp;
    }
}
