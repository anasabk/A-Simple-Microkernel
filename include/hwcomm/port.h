#ifndef __HWCOMM__PORT_H__
#define __HWCOMM__PORT_H__

#include "common/types.h"

namespace microkernel
{
    class Port
    {
    protected:
        Port(uint16_t port_num);
        // FIXME: Must be virtual (currently isnt because the kernel has no memory management yet)
        ~Port();

        uint16_t port_num;
    };


    class Port8Bit : public Port
    {
    public:
        Port8Bit(uint16_t portnumber);
        ~Port8Bit();

        virtual uint8_t read();
        virtual void write(uint8_t data);

    protected:
        static inline uint8_t read8(uint16_t _port)
        {
            uint8_t result;
            __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (_port));
            return result;
        }

        static inline void write8(uint16_t _port, uint8_t _data)
        {
            __asm__ volatile("outb %0, %1" : : "a" (_data), "Nd" (_port));
        }
    };



    class Port8BitSlow : public Port8Bit
    {
    public:
        Port8BitSlow(uint16_t portnumber);
        ~Port8BitSlow();

        virtual void write(uint8_t data);

    protected:
        static inline void write8_slow(uint16_t _port, uint8_t _data)
        {
            __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (_data), "Nd" (_port));
        }

    };



    class Port16Bit : public Port
    {
    public:
        Port16Bit(uint16_t portnumber);
        ~Port16Bit();

        virtual uint16_t read();
        virtual void write(uint16_t data);

    protected:
        static inline uint16_t read16(uint16_t _port)
        {
            uint16_t result;
            __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (_port));
            return result;
        }

        static inline void write16(uint16_t _port, uint16_t _data)
        {
            __asm__ volatile("outw %0, %1" : : "a" (_data), "Nd" (_port));
        }
    };



    class Port32Bit : public Port
    {
    public:
        Port32Bit(uint16_t portnumber);
        ~Port32Bit();

        virtual uint32_t read();
        virtual void write(uint32_t data);

    protected:
        static inline uint32_t read32(uint16_t _port)
        {
            uint32_t result;
            __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (_port));
            return result;
        }

        static inline void write32(uint16_t _port, uint32_t _data)
        {
            __asm__ volatile("outl %0, %1" : : "a"(_data), "Nd" (_port));
        }
    };
    
} // namespace myos

#endif