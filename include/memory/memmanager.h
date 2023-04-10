#ifndef __MEMORY_MANAGEMENT_H__
#define __MEMORY_MANAGEMENT_H__

#include "common/types.h"
#include "gdt.h"


namespace microkernel
{
    struct MemChunk {
        size_t size;
        MemChunk* next_chunk;
        bool is_free;
    } __attribute__((packed));

    class MemManager
    {
    public:
        MemManager(uint16_t segement_sl);
        ~MemManager();

        /**
         * @brief Get the base of the memory segment. (aka max address) 
         */
        uint32_t get_base();

        /**
         * @brief Get the base of the memory segment. (aka min address) 
         */
        uint32_t get_limit();

        /**
         * @brief Return the base address of a new memory chunk.
         * 
         * @param size The size of the new chunk.
         * @return void* The base address of the memory chunk. 
         */
        virtual void* get_chunk(uint32_t size);

        /**
         * @brief Free the given memory chunk to be used later.
         * 
         * @param chunk_p The pointer to the base of the memory 
         * chunk to free.
         */
        void free(void* chunk_p);


    private:
        /**
         * @brief Segment Sellector of the memory of this manager.
         */
        uint16_t segment_sl;
        
        /**
         * @brief The pointer to the limit of the 
         * last allocated chunk.
         */
        uint32_t last_p;
        
        /**
         * @brief The list of pointers to the bases of 
         * the none used chunks free'd previously.
         */
        MemChunk* free_list;

    };
    
} // namespace microkernel


#endif
