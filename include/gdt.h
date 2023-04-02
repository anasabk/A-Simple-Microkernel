#ifndef __MYOS__GDT_H_
#define __MYOS__GDT_H_

#include "common/types.h"
#include "iolib.h"

namespace microkernel
{
    class GlobalDescriptorTable {
    protected:
        class SegmentDescriptor {
        private:
            uint16_t limit_lo;
            uint16_t base_lo;
            uint8_t limit_hi;
            uint8_t type;
            uint8_t flags_limit_hi;
            uint8_t base_vhi;
        
        public:
            SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t flags);
            uint32_t get_base();
            uint32_t get_limit();
            
        } __attribute__((packed));

        SegmentDescriptor null_ss;
        SegmentDescriptor unused_ss;
        SegmentDescriptor code_ss;
        SegmentDescriptor data_ss;
        SegmentDescriptor stack_ss;

    public:
        GlobalDescriptorTable();
        ~GlobalDescriptorTable();

        void init();

        /**
         * @brief Get the Code Segment Selector.
         */
        uint16_t get_css();

        /**
         * @brief Get the Data Segment Selector.
         */
        uint16_t get_dss();

        /**
         * @brief Get the Stack Segment Selector.
         */
        uint16_t get_sss();
        
    };

} // namespace myos

#endif