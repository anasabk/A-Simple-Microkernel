#ifndef __MOUSE_H__
#define __MOUSE_H__

#include "common/types.h"
#include "hwcomm/port.h"
#include "hwcomm/interrupts.h"

namespace microkernel
{
    class MouseDriver : public InterruptHandler
    {
        Port8Bit data_port;
        Port8Bit command_port;
        uint8_t buffer[3];
        uint8_t offset;
        uint8_t buttons;

        int8_t x, y;
    public:
        MouseDriver(InterruptManager* manager);
        ~MouseDriver();
        virtual uint32_t handle_interrupt(uint32_t esp);
    };
} // namespace microkernel

#endif