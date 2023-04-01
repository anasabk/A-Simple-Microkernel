#include "../../include/hwcomm/port.h"

using namespace microkernel;
using namespace microkernel::hwcomm;


Port::Port(uint16_t port_num)
{
    this->port_num = port_num;
}

Port::~Port()
{
}



Port8Bit::Port8Bit(uint16_t port_num)
    : Port(port_num) {}

Port8Bit::~Port8Bit()
{
}

void Port8Bit::write(uint8_t data)
{
    write8(port_num, data);
}

uint8_t Port8Bit::read()
{
    return read8(port_num);
}



Port8BitSlow::Port8BitSlow(uint16_t port_num)
    : Port8Bit(port_num)
{
}

Port8BitSlow::~Port8BitSlow()
{
}

void Port8BitSlow::write(uint8_t data)
{
    write8_slow(port_num, data);
}



Port16Bit::Port16Bit(uint16_t port_num)
    : Port(port_num)
{
}

Port16Bit::~Port16Bit()
{
}

void Port16Bit::write(uint16_t data)
{
    write16(port_num, data);
}

uint16_t Port16Bit::read()
{
    return read16(port_num);
}



Port32Bit::Port32Bit(uint16_t port_num)
    : Port(port_num)
{
}

Port32Bit::~Port32Bit()
{
}

void Port32Bit::write(uint32_t data)
{
    write32(port_num, data);
}

uint32_t Port32Bit::read()
{
    return read32(port_num);
}
