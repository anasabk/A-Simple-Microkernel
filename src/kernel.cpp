#include "kernel.h"


using namespace microkernel;

Kernel::Kernel()
{
}

Kernel::~Kernel()
{
}

uint32_t buyaba[128];
uint32_t buya = 0;

void foo(){
    printf("\nfoo\n");
    int32_t i;

    printf("initialized global(data segment): ");
    printfHex32((uint32_t) &buya);
    printf("\nlocal variable (stack segment): ");
    printfHex32((uint32_t) &i);
    printf("\nfunction address (code segment): ");
    printfHex32((uint32_t) &foo);

    while(1)
        printf("1");
}

void foo2(){
    printf("\nfoo2\n");
    int32_t i;

    printf("initialized global(data segment): ");
    printfHex32((uint32_t) &buya);
    printf("\nlocal variable (stack segment): ");
    printfHex32((uint32_t) &i);
    printf("\nfunction address (code segment): ");
    printfHex32((uint32_t) &foo2);

    while(1)
        printf("2");
}

void Kernel::run() 
{
    uint32_t buya2 = 1;

    GlobalDescriptorTable l_gdt;
    gdt = &l_gdt;

    InterruptManager l_interrupt_manager(0x20, gdt);
    interrupt_manager = &l_interrupt_manager;

    Taskmanager l_task_manager(interrupt_manager);
    task_manager = &l_task_manager;

    MemManager l_mem_manager(gdt);
    mem_manager = &l_mem_manager;

    // printf("initialized global(data segment): ");
    // printfHex32((uint32_t) &buya);
    // printf("\ninitialized global(data segment): ");
    // printfHex32((uint32_t) &buyaba);
    // printf("\nlocal variable (stack segment): ");
    // printfHex32((uint32_t) &buya2);
    // printf("\nfunction address (code segment): ");
    // printfHex32((uint32_t) &foo);

    gdt->init();

    printf("\nGlobal Descriptor Table:\nCode Segment: ");
    printfHex16(gdt->get_css());
    printf("\nData Segment:");
    printfHex16(gdt->get_dss());
    printf("\nStack Segment:");
    printfHex16(gdt->get_sss());
    printf("\n");

    printf("\nMemory Manager:\n Base: ");
    printfHex32(mem_manager->get_base());
    printf("\nLimit:");
    printfHex32(mem_manager->get_limit());
    printf("\n");


    // Taskmanager task_manager(interrupt_manager);

    printf("Initilizing task foo:");
    void* chunk = mem_manager->get_stack_chunk(4096);
    printfHex32((uint32_t)chunk);
    printf("\n");
    Task task_foo(gdt, foo, 4096, chunk);

    printf("Initilizing task foo2:");
    chunk = mem_manager->get_stack_chunk(4096);
    printfHex32((uint32_t)chunk);
    printf("\n");
    Task task_foo2(gdt, foo2, 4096, chunk);

    printf("Adding tasks\n");
    task_manager->add_task(&task_foo);
    task_manager->add_task(&task_foo2);
    
    Keyboard my_keyboard(interrupt_manager);
    
    printf("starting interrupts\n");
    interrupt_manager->activate();

    while(1);
}
