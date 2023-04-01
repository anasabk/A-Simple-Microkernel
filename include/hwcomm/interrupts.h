#ifndef __MYOS__HWCOMM__INTERRUPTMANAGER_H
#define __MYOS__HWCOMM__INTERRUPTMANAGER_H

#include "gdt.h"
#include "common/types.h"
#include "port.h"
// #include "../multitasking.h"

#define IDT_INTERRUPT_GATE 0xE


namespace microkernel
{
    namespace hwcomm
    {
        class InterruptManager;

        class InterruptHandler
        {
        protected:
            uint8_t interrupt_num;
            InterruptManager* interrupt_manager;

            InterruptHandler(
                InterruptManager* interrupt_manager, 
                uint8_t interrupt_num
            );

            ~InterruptHandler();

        public:
            virtual uint32_t handle_interrupt(uint32_t esp);
        };


        class InterruptManager
        {
        friend class InterruptHandler;
        protected:
            struct GateDescriptor
            {
                uint16_t handler_addr_lo;
                uint16_t gdt_css;   // General Descriptor Table Code Segment Selector
                uint8_t reserved;
                uint8_t access;
                uint16_t handler_addr_hi;
            } __attribute__((packed));

            struct InterruptDescriptorTablePointer
            {
                uint16_t size;
                uint32_t base;
            } __attribute__((packed));


            InterruptHandler* handlers[256];

            static GateDescriptor idt[256]; //Interrupt Descriptor Table

            uint16_t hardware_interrupt_offset;

            static InterruptManager* active_interrupt_manager;

            /**
             * @brief Set an entry in the interrupt descriptor table
             * with the given information.
             * 
             * @param interrupt The number of the interrupt .
             * @param css_offset The interrupt's code segment 
             * selector offset (from gdt).
             * @param handler The function that handles the interrupt.
             * @param descriptor_privilege_level Privilege level.
             * @param descriptor_type Descriptor type.
             */
            void set_idt_entry(
                uint8_t interrupt,
                uint16_t css_offset, 
                void (*handler)(),
                uint8_t descriptor_privilege_level, 
                uint8_t descriptor_type);

            static void ignore_interrupt();

            static void handle_interrupt_request_0x00();
            static void handle_interrupt_request_0x01();
            static void handle_interrupt_request_0x02();
            static void handle_interrupt_request_0x03();
            static void handle_interrupt_request_0x04();
            static void handle_interrupt_request_0x05();
            static void handle_interrupt_request_0x06();
            static void handle_interrupt_request_0x07();
            static void handle_interrupt_request_0x08();
            static void handle_interrupt_request_0x09();
            static void handle_interrupt_request_0x0A();
            static void handle_interrupt_request_0x0B();
            static void handle_interrupt_request_0x0C();
            static void handle_interrupt_request_0x0D();
            static void handle_interrupt_request_0x0E();
            static void handle_interrupt_request_0x0F();
            static void handle_interrupt_request_0x31();
            
            // System call interrupt
            static void handle_interrupt_request_0x80();

            static void handle_exception_0x00();
            static void handle_exception_0x01();
            static void handle_exception_0x02();
            static void handle_exception_0x03();
            static void handle_exception_0x04();
            static void handle_exception_0x05();
            static void handle_exception_0x06();
            static void handle_exception_0x07();
            static void handle_exception_0x08();
            static void handle_exception_0x09();
            static void handle_exception_0x0A();
            static void handle_exception_0x0B();
            static void handle_exception_0x0C();
            static void handle_exception_0x0D();
            static void handle_exception_0x0E();
            static void handle_exception_0x0F();
            static void handle_exception_0x10();
            static void handle_exception_0x11();
            static void handle_exception_0x12();
            static void handle_exception_0x13();

            /**
             * @brief Handle the given interrupt.
             * 
             * @param interrupt The interrupt number.
             * @param esp Stack pointer from before the interrupt.
             * @return The new CPU state
             */
            uint32_t handle_interrupt(uint8_t interrupt, uint32_t esp);

            static uint32_t receive_interrupt(uint8_t interrupt, uint32_t esp);

            Port8BitSlow pic_master_command;
            Port8BitSlow pic_master_data;
            Port8BitSlow pic_slave_command;
            Port8BitSlow pic_slave_data;

        public:
            /**
             * @brief Construct a new Interrupt Manager object.
             * 
             * @param hw_interrupt_offset Hardware interrupt offset. 
             * @param gdt Global descriptor table.
             */
            InterruptManager(
                uint16_t hw_interrupt_offset, 
                GlobalDescriptorTable* gdt
            );
                                
            ~InterruptManager();

            /**
             * @brief Getter for the Hardware Interrupt Offset.
             */
            uint16_t get_hw_interrupt_offset();
            
            void activate();
            void deactivate();
        };
    } // namespace hwcomm
    
} // namespace microkernel

#endif
