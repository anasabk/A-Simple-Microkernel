#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "hwcomm/interrupts.h"
#include "multitasking/processmanager.h"


namespace microkernel
{
    class KeyboardDriver : public InterruptHandler
    {
    public:
        KeyboardDriver(InterruptManager* manager);
        ~KeyboardDriver();

        virtual uint32_t handle_interrupt(uint32_t esp);

    private:
        Port8Bit data_port;
        Port8Bit command_port;
    };
    
} // namespace microkernel

#endif
