#include "hwcomm/interrupts.h"


using namespace microkernel;

InterruptHandler::InterruptHandler(
    InterruptManager* t_interrupt_manager, 
    uint8_t t_interrupt_num)
    : interrupt_num(t_interrupt_num), 
      interrupt_manager(t_interrupt_manager)
{
    interrupt_manager->handlers[interrupt_num] = this;
}

InterruptHandler::~InterruptHandler()
{
    if(interrupt_manager->handlers[interrupt_num] == this)
        interrupt_manager->handlers[interrupt_num] = 0;
}

uint32_t InterruptHandler::handle_interrupt(uint32_t t_esp)
{
    return t_esp;
}

void InterruptHandler::set_manager(InterruptManager* t_manager)
{
    interrupt_manager = t_manager;
    t_manager->handlers[interrupt_num] = this;
}




InterruptManager::GateDescriptor InterruptManager::idt[256];
InterruptManager* InterruptManager::active_interrupt_manager = nullptr;

void InterruptManager::set_idt_entry(
        uint8_t t_interrupt,
        uint16_t t_css_offset, 
        void (*t_handler)(),
        uint8_t t_descriptor_privilege_level, 
        uint8_t t_descriptor_type)
{
    // address of pointer to code segment (relative to global descriptor table)
    // and address of the handler (relative to segment)
    idt[t_interrupt].handler_addr_lo = ((uint32_t) t_handler) & 0xFFFF;
    idt[t_interrupt].handler_addr_hi = (((uint32_t) t_handler) >> 16) & 0xFFFF;
    idt[t_interrupt].gdt_css = t_css_offset;

    const uint8_t IDT_DESC_PRESENT = 0x80;

    idt[t_interrupt].access = 
        IDT_DESC_PRESENT | 
        ((t_descriptor_privilege_level & 3) << 5) | 
        t_descriptor_type;
    
    idt[t_interrupt].reserved = 0;
}


InterruptManager::InterruptManager(
        uint16_t t_hardware_interrupt_offset, 
        GlobalDescriptorTable* t_gdt)
    : pic_master_command(0x20),
      pic_master_data(0x21),
      pic_slave_command(0xA0),
      pic_slave_data(0xA1)
{
    this->hardware_interrupt_offset = t_hardware_interrupt_offset;
    uint32_t code_segment = t_gdt->get_css();

    for(uint8_t i = 255; i > 0; --i)
    {
        set_idt_entry(i, code_segment, &ignore_interrupt, 0, IDT_INTERRUPT_GATE);
        handlers[i] = 0;
    }
    set_idt_entry(0, code_segment, &ignore_interrupt, 0, IDT_INTERRUPT_GATE);
    handlers[0] = 0;

    set_idt_entry(0x00, code_segment, &handle_exception_0x00, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x01, code_segment, &handle_exception_0x01, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x02, code_segment, &handle_exception_0x02, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x03, code_segment, &handle_exception_0x03, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x04, code_segment, &handle_exception_0x04, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x05, code_segment, &handle_exception_0x05, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x06, code_segment, &handle_exception_0x06, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x07, code_segment, &handle_exception_0x07, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x08, code_segment, &handle_exception_0x08, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x09, code_segment, &handle_exception_0x09, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x0A, code_segment, &handle_exception_0x0A, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x0B, code_segment, &handle_exception_0x0B, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x0C, code_segment, &handle_exception_0x0C, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x0D, code_segment, &handle_exception_0x0D, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x0E, code_segment, &handle_exception_0x0E, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x0F, code_segment, &handle_exception_0x0F, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x10, code_segment, &handle_exception_0x10, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x11, code_segment, &handle_exception_0x11, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x12, code_segment, &handle_exception_0x12, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(0x13, code_segment, &handle_exception_0x13, 0, IDT_INTERRUPT_GATE);

    set_idt_entry(hardware_interrupt_offset + 0x00, code_segment, &handle_interrupt_request_0x00, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x01, code_segment, &handle_interrupt_request_0x01, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x02, code_segment, &handle_interrupt_request_0x02, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x03, code_segment, &handle_interrupt_request_0x03, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x04, code_segment, &handle_interrupt_request_0x04, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x05, code_segment, &handle_interrupt_request_0x05, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x06, code_segment, &handle_interrupt_request_0x06, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x07, code_segment, &handle_interrupt_request_0x07, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x08, code_segment, &handle_interrupt_request_0x08, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x09, code_segment, &handle_interrupt_request_0x09, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x0A, code_segment, &handle_interrupt_request_0x0A, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x0B, code_segment, &handle_interrupt_request_0x0B, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x0C, code_segment, &handle_interrupt_request_0x0C, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x0D, code_segment, &handle_interrupt_request_0x0D, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x0E, code_segment, &handle_interrupt_request_0x0E, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(hardware_interrupt_offset + 0x0F, code_segment, &handle_interrupt_request_0x0F, 0, IDT_INTERRUPT_GATE);
    set_idt_entry(                            0x80, code_segment, &handle_interrupt_request_0x80, 0, IDT_INTERRUPT_GATE);

    pic_master_command.write(0x11);
    pic_slave_command.write(0x11);

    // remap
    pic_master_data.write(hardware_interrupt_offset);
    pic_slave_data.write(hardware_interrupt_offset + 8);

    pic_master_data.write(0x04);
    pic_slave_data.write(0x02);

    pic_master_data.write(0x01);
    pic_slave_data.write(0x01);

    pic_master_data.write(0x00);
    pic_slave_data.write(0x00);

    InterruptDescriptorTablePointer idt_pointer;
    idt_pointer.size  = 256*sizeof(GateDescriptor) - 1;
    idt_pointer.base  = (uint32_t)idt;
    asm volatile("lidt %0" : : "m" (idt_pointer));
}

InterruptManager::~InterruptManager()
{
    deactivate();
}

uint16_t InterruptManager::get_hw_interrupt_offset()
{
    return hardware_interrupt_offset;
}

void InterruptManager::activate()
{
    if(active_interrupt_manager != 0)
        active_interrupt_manager->deactivate();

    active_interrupt_manager = this;
    asm("sti");
}

void InterruptManager::deactivate()
{
    if(active_interrupt_manager == this)
    {
        active_interrupt_manager = 0;
        asm("cli");
    }
}

uint32_t InterruptManager::receive_interrupt(uint8_t interrupt, uint32_t esp)
{
    if(active_interrupt_manager != 0)
        return active_interrupt_manager->handle_interrupt(interrupt, esp);
    return esp;
}

uint32_t InterruptManager::handle_interrupt(uint8_t interrupt, uint32_t esp)
{
    if(handlers[interrupt] != 0) {
        esp = handlers[interrupt]->handle_interrupt(esp);
    
    } else if(interrupt != hardware_interrupt_offset) {
        printf("UNHANDLED INTERRUPT: 0x");
        printfHex(interrupt);
        printf("\n");
    }

    if (interrupt == 0x0D) {
        printf("General Protection Interrupt: ");
        printfHex32(((CPUState*)esp)->error);
        printf("\n");
        asm("int $0x20");
    } else if (interrupt == 0x06) {
        printf("Invalid Instruction exception, eip is: 0x");
        printfHex32(((CPUState*)esp)->eip);
        printf("\n");
    }

    // hardware interrupts must be acknowledged
    if (hardware_interrupt_offset <= interrupt && 
        interrupt < hardware_interrupt_offset + 16)
    {
        pic_master_command.write(0x20);
        if(hardware_interrupt_offset + 8 <= interrupt)
            pic_slave_command.write(0x20);
    }

    return esp;
}
