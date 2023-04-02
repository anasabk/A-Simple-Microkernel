#ifndef __MEMORY_MANAGEMENT_H__
#define __MEMORY_MANAGEMENT_H__

#include "common/types.h"
#include "gdt.h"

namespace microkernel
{
    struct StackChunk {
        // uint32_t base_addr;
        size_t size;
        StackChunk* next_chunk;
        bool is_free;
        //The actual stack lies beyond
    } __attribute__((packed));

    class MemManager
    {
    public:
        MemManager(GlobalDescriptorTable* gdt);
        ~MemManager();

        void* get_stack_chunk(uint32_t size);

        uint32_t get_base();
        uint32_t get_limit();

        void free(void* stack_p);

    private:
        uint16_t stack_ss;
        uint32_t last_stack_p;
        StackChunk* free_list;

    };
    
} // namespace microkernel


#endif
