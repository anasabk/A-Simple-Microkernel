#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "hwcomm/interrupts.h"


namespace microkernel
{
    class Keyboard : public InterruptHandler
    {
    public:
        Keyboard(InterruptManager* manager);
        ~Keyboard();

        virtual uint32_t handle_interrupt(uint32_t esp);

    private:
        Port8Bit data_port;
        Port8Bit command_port;
    };
    
} // namespace microkernel

#endif
