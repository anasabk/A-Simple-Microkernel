#include "kernel.h"


using namespace microkernel;

Kernel* Kernel::active_kernel;

Kernel::Kernel()
    : 
      InterruptHandler(
        &interrupt_manager,
        SYSCALL_INT_NUM),
      interrupt_manager(HARDWARE_INT_OFFSET, &gdt),
      gdt(),
      task_manager(&interrupt_manager),
      mem_manager(&gdt),
      keyboard_driver(&interrupt_manager)
{
    set_manager(&interrupt_manager);
    active_kernel = this;
}

Kernel::~Kernel()
{
}

uint16_t Kernel::get_pid() 
{
    return active_kernel->task_manager.current_task;
}

uint32_t Kernel::handle_interrupt(uint32_t esp) 
{
    CPUState* cpu = (CPUState*)esp;

    switch (cpu->eax)
    {
    case 2: {
        printf("instruction pointer: ");
        printfHex32(cpu->eip);
        printf("\n");
        dump_cpu((CPUState*)cpu);
        fork_task(esp, (void*)cpu->ecx);
    }
    break;

    case 4:
        printf((char*)cpu->ebx);
        break;

    case 11: {
        printf("instruction pointer: ");
        printfHex32(cpu->eip);
        printf("\n");
        execve_task(esp, (char*)cpu->ebx);
    }
    break;
    
    case 190: {
        printf("instruction pointer: ");
        printfHex32(cpu->eip);
        printf("\n");
        vfork_task(esp, (void*)cpu->ecx);
    }
    break;

    default:
        break;
    }

    return esp;
}

void Kernel::fork_task(uint32_t esp, void* t_return_addr) {
    printf("forking\n");
    CPUState* cpu = (CPUState*)esp;
    Task* current_task = 
        task_manager.tasks[task_manager.current_task];

    Task* new_task = 
            (Task*)(mem_manager.get_stack_chunk(
                    current_task->get_stack_size() + sizeof(Task)
            ) - sizeof(Task));

    new_task->fork(
        &gdt,
        (uint8_t*)new_task,
        t_return_addr,
        cpu,
        current_task
    );

    task_manager.add_task(new_task);

    cpu->ebx = new_task->get_pid();
}

Task* Kernel::create_task(void* t_entry_point, uint32_t t_stack_size) {
    Task* current_task = 
        task_manager.tasks[task_manager.current_task];

    Task* new_task = 
            (Task*)(mem_manager.get_stack_chunk(
                    t_stack_size + sizeof(Task)
            ) - sizeof(Task));

    new_task->setup(
        &gdt, 
        t_entry_point, 
        (uint8_t*)new_task, 
        t_stack_size, 
        current_task
    );

    task_manager.add_task(new_task);

    return new_task;
}

void Kernel::vfork_task(uint32_t t_esp, void* t_return_addr) {
    printf("vforking\n");
    CPUState* cpu = (CPUState*)t_esp;
    Task* current_task = 
        task_manager.tasks[task_manager.current_task];

    Task* new_task = 
            (Task*)(mem_manager.get_stack_chunk(
                    current_task->get_stack_size() + sizeof(CPUState) + sizeof(Task)
            ) - sizeof(Task));

    new_task->vfork(
        &gdt,
        (uint8_t*)new_task,
        t_return_addr,
        cpu,
        current_task
    );

    task_manager.add_task(new_task);

    cpu->ebx = new_task->get_pid();
} 

void Kernel::execve_task(uint32_t esp, const char* pathname) {
    printf("changing execution\n");
    CPUState* cpu = (CPUState*)esp;
    Task* current_task = 
        task_manager.tasks[task_manager.current_task];

    current_task->execve(&gdt, (void*) pathname);
}


uint32_t buyaba[128];
uint32_t buya = 0;

void foo(){
    printf("foo\n");
    int8_t i[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    printf_int(Kernel::fork());

    printfHex32((uint32_t)i);
    // printfHex32(i2);

    for(int j = 0; j < 9; j++)
        printf_int(i[j]);
    printf(" foo again\n");
    while(1);
        // printf("1");
}

void foo2(){
    printf("\nfoo2\n");
    int8_t i1;

    // printf("initialized global(data segment): ");
    // printfHex32((uint32_t) &buya);
    // printf("\nlocal variable (stack segment)  : ");
    // printfHex32((uint32_t) &i1);
    // printf("\nfunction address (code segment) : ");
    // printfHex32((uint32_t) &foo2);
    // printf("\n");

    printf("foo2 again\n");
    while(1);
        // printf("2");
}

int32_t binary_search(int32_t* arr, size_t len, int32_t target) {
    if(len == 0)
        return -1;

    if(arr[len/2] == target)
        return len/2;

    if(arr[len/2] > target)
        return binary_search(arr, len/2, target);
    else
        return len/2 + binary_search(&arr[len/2], len/2, target);
}

int32_t linear_search(int32_t* arr, size_t len, int32_t target) {
    for(int i = 0; i < len; i++)
        if(arr[i] == target)
            return i;

    return -1;
}

void collatz_sequence(uint32_t num) {

}

void task_collatz_sequence() {
    for(int8_t i = 25; i > 0; i--)
        collatz_sequence(i);
}

void task_binary_search() {
    int32_t arr[] = {10, 20, 30, 50, 60, 80, 100, 110, 130, 170};
    size_t length = 10;
    // Kernel::fork();
    printf("binary search result: ");
    printf_int(binary_search(arr, length, 110));
    printf("\n");

    while(1);
}

void init() {
    char* buffer = "                      ";
    printf("init entered\n");
    uint8_t i = 1;

    if(Kernel::vfork() == 0){
        i++;
        printfHex32((uint32_t) &i);
        printf(" ");
        printf_int(i);
        printf(" result = 0\n");
        Kernel::execve((char*)foo, nullptr, nullptr);
    } else {
        i++;
        printfHex32((uint32_t) &i);
        printf(" ");
        printf_int(i);
        printf(" result = 1\n");
    }

    printf(" init inside\n");

    while(1);
}

void Kernel::run() 
{
    // GlobalDescriptorTable l_gdt;
    // gdt = &l_gdt;

    // InterruptManager l_interrupt_manager(0x20, gdt);
    // interrupt_manager = &l_interrupt_manager;

    // Taskmanager l_task_manager(interrupt_manager);
    // task_manager = &l_task_manager;

    // MemManager l_mem_manager(gdt);
    // mem_manager = &l_mem_manager;

    // printf("initialized global(data segment): ");
    // printfHex32((uint32_t) &buya);
    // printf("\ninitialized global(data segment): ");
    // printfHex32((uint32_t) &buyaba);
    // printf("\nlocal variable (stack segment): ");
    // printfHex32((uint32_t) &buya2);
    // printf("\nfunction address (code segment): ");
    // printfHex32((uint32_t) &foo);

    gdt.init();
    

    // printf("Global Descriptor Table:\nCode Segment: ");
    // printfHex16(gdt.get_css());
    // printf("\nData Segment:");
    // printfHex16(gdt.get_dss());
    // printf("\nStack Segment:");
    // printfHex16(gdt.get_sss());
    // printf("\n");

    // printf("Memory Manager:\nBase : ");
    // printfHex32(mem_manager.get_base());
    // printf("\nLimit: ");
    // printfHex32(mem_manager.get_limit());
    // printf("\n");

    // printf("Initilizing task foo :");
    // void* chunk = mem_manager->get_stack_chunk(4096);
    // printfHex32((uint32_t)chunk);
    // printf("\n");
    // Task task_foo(gdt, foo, 4096, mem_manager);
    // create_task((void*)foo, 4096);

    // printf("Initilizing task foo2:");
    // chunk = mem_manager->get_stack_chunk(4096);
    // printfHex32((uint32_t)chunk);
    // printf("\n");
    // Task task_foo2(gdt, foo2, 4096, mem_manager);
    // create_task((void*)foo2, 4096);

    // printf("Adding tasks\n");
    // task_manager->add_task(&task_foo);
    // task_manager->add_task(&task_foo2);
    
    interrupt_manager.activate();
    // printf("Interrupt manager activated\n");

    create_task((void*)foo, 4096);

    while(1);
}

volatile uint16_t Kernel::fork()
{
    uint32_t result_child;
    asm("int $0x80" : "=b" (result_child) : "a" (2), "c" (__builtin_return_address(0)));
    return result_child;
}

volatile uint16_t Kernel::vfork() 
{
    uint32_t result_child;
    asm("int $0x80" : "=b" (result_child) : "a" (190), "c" (__builtin_return_address(0)));
    return result_child;
}

void Kernel::sysprintf(char* str) {
    asm("int $0x80" : : "a" (4), "b" (str));
}

int Kernel::execve(const char *pathname, char *const argv[], char *const envp[]) {
    asm("int $0x80" : : "a" (11), "b" (pathname), "c" (argv), "d" (envp));
}
