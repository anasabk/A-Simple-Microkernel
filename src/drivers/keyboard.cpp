#include "drivers/keyboard.h"
#include "kernel.h"


using namespace microkernel;

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
    : InterruptHandler(manager, 0x21),
      data_port(0x60),
      command_port(0x64)
{
    while(command_port.read() & 0x1)
        data_port.read();

    command_port.write(0xAE); //activate interrupts
    command_port.write(0x20);
    uint8_t status = (data_port.read() | 1) & ~0x10;
    command_port.write(0x60);
    data_port.write(status);
    data_port.write(0xF4);
}

KeyboardDriver::~KeyboardDriver()
{
}

void add_to_buffer(char c) {
    if(buf_index >= 255)
        return;
    buf_index++; 
    in_buffer[buf_index] = c; 
}

uint32_t KeyboardDriver::handle_interrupt(uint32_t esp) {
    uint8_t key = data_port.read();
    char temp[2];
    char* buffer;

    if(key < 0x80) {
        switch(key) {
            case 0xFA: break;
            case 0x45: case 0xC5: break;

            case 0x1C: 
                ready = 1; 
                add_to_buffer(0);
                printf("\n");
                break;

            case 0x02: /*printf("1"); break;*/
            case 0x03: /*printf("2"); break;*/
            case 0x04: /*printf("3"); break;*/
            case 0x05: /*printf("4"); break;*/
            case 0x06: /*printf("5"); break;*/
            case 0x07: /*printf("6"); break;*/
            case 0x08: /*printf("7"); break;*/
            case 0x09: /*printf("8"); break;*/
            case 0x0A: /*printf("9"); break;*/
                printf(key + 47);
                add_to_buffer(key + 47);
                break;

            case 0x0B: /*printf("0"); break;*/
                printf('0');
                add_to_buffer('0');
                break;

            case 0x10: 
                printf('q');
                add_to_buffer('q');
                break;
    
            case 0x11: 
                printf('w');
                add_to_buffer('w');
                break;

            case 0x12: 
                printf('e');
                add_to_buffer('e');
                break;

            case 0x13: 
                printf('r');
                add_to_buffer('r');
                break;

            case 0x14: 
                printf('t');
                add_to_buffer('t');
                break;

            case 0x15: 
                printf('z');
                add_to_buffer('z');
                break;

            case 0x16: 
                printf('u');
                add_to_buffer('u');
                break;

            case 0x17: 
                printf('i');
                add_to_buffer('i');
                break;

            case 0x18: 
                printf('o');
                add_to_buffer('o');
                break;

            case 0x19: 
                printf('p');
                add_to_buffer('p');
                break;


            case 0x1E: 
                printf('a');
                add_to_buffer('a');
                break;

            case 0x1F: 
                printf('s');
                add_to_buffer('s');
                break;

            case 0x20: 
                printf('d');
                add_to_buffer('d');
                break;

            case 0x21: 
                printf('f');
                add_to_buffer('f');
                break;

            case 0x22: 
                printf('g');
                add_to_buffer('g');
                break;

            case 0x23: 
                printf('h');
                add_to_buffer('h');
                break;

            case 0x24: 
                printf('j');
                add_to_buffer('j');
                break;

            case 0x25: 
                printf('k');
                add_to_buffer('k');
                break;

            case 0x26: 
                printf('l');
                add_to_buffer('l');
                break;


            case 0x2C: 
                printf('y');
                add_to_buffer('y');
                break;

            case 0x2D: 
                printf('x');
                add_to_buffer('x');
                break;

            case 0x2E: 
                printf('c');
                add_to_buffer('c');
                break;

            case 0x2F: 
                printf('v');
                add_to_buffer('v');
                break;

            case 0x30: 
                printf('b');
                add_to_buffer('b');
                break;

            case 0x31: 
                printf('n');
                add_to_buffer('n');
                break;

            case 0x32: 
                printf('m');
                add_to_buffer('m');
                break;

            case 0x33: 
                printf(',');
                add_to_buffer(',');
                break;

            case 0x34: 
                printf('.');
                add_to_buffer('.');
                break;

            case 0x35: 
                printf('-');
                add_to_buffer('-');
                break;

            case 0x39: 
                printf(' ');
                add_to_buffer(' ');
                break;
        
            // TAB
            case 0x0F: printf("\n"); 
                return Kernel::schedule(esp);
                break;

            // BACKSPACE
            case 0x0E: 
                in_buffer[buf_index] = 0; 
                if(buf_index>=0) 
                    buf_index--; 
                printf("\b");;
                break;

            // UNKOWN CHARACTER
            default:
                char* foo = "KEYBOARD 0x00";
                char* hex = "0123456789ABCDEF";
                foo[11] = hex[(key >> 4) & 0xF];
                foo[12] = hex[key & 0xF];
                printf(foo);
                break;
        }
    }

    return esp;
}