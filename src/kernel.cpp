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
      process_manager(&interrupt_manager, &gdt),
      ss_manager(gdt.get_sss()),
      keyboard_driver(&interrupt_manager)
{
    set_manager(&interrupt_manager);
    active_kernel = this;
}

Kernel::~Kernel()
{
}

uint16_t Kernel::get_current_pid() 
{
    return active_kernel->process_manager.get_current_process()->get_pid();
}

uint32_t Kernel::handle_interrupt(uint32_t esp) 
{
    CPUState* cpu = (CPUState*)esp;

    switch (cpu->eax)
    {
    case 1: {
        sys_exit();
        return (uint32_t)process_manager.schedule(cpu);
    }
    break;

    case 2:
        cpu->ebx = sys_fork(esp, (void*)cpu->ecx);
        break;

    case 4:
        printf((char*)cpu->ebx);
        break;

    case 7: 
        cpu->edx = (uint32_t)sys_waitpid((int16_t)cpu->ebx, (ProcessState)cpu->ecx);
        break;

    case 11:
        return (uint32_t)sys_execve(esp, (char*)cpu->ebx, (char**)cpu->ecx);
        break;

    default:
        break;
    }

    return esp;
}

uint8_t Kernel::create_task(void* t_entry_point, uint32_t t_stack_size) {
    Process* current_task = 
        process_manager.get_current_process();

    Process* new_task = process_manager.get_process_mem();
    
    uint8_t* new_stack = 
        (uint8_t*)ss_manager.get_chunk(t_stack_size) + t_stack_size - 1;

    new_task->setup(
        &gdt, 
        t_entry_point,
        new_stack, 
        t_stack_size, 
        current_task
    );

    process_manager.add_proc(new_task);

    return new_task->get_pid();
}

uint8_t Kernel::sys_fork(uint32_t esp, void* t_return_addr) {
    CPUState* cpu = (CPUState*)esp;

    Process* current_task = 
        process_manager.get_current_process();

    Process* new_task = process_manager.get_process_mem();
    
    uint8_t* new_stack = 
        (uint8_t*)ss_manager.get_chunk(current_task->get_stack_size()) + 
        current_task->get_stack_size() - 1;

    new_task->fork(
        &gdt,
        new_stack,
        t_return_addr,
        cpu,
        current_task
    );

    process_manager.add_proc(new_task);

    return new_task->get_pid();
}

CPUState* Kernel::sys_execve(uint32_t esp, char *const pathname, char *const argv[]) {
    // printf("changing execution\n");
    CPUState* cpu = (CPUState*)esp;
    Process* current_task = 
        process_manager.get_current_process();

    return current_task->execve(cpu, &gdt, (void**)argv, (void*)pathname);
}

void Kernel::sys_exit() {
    ss_manager.free(process_manager.exit_proc(get_current_pid()));
}

int16_t Kernel::sys_waitpid(int16_t pid, ProcessState state) {
    // printf("Waiting\n");
    Process* current_task = 
        process_manager.get_current_process();

    Message* msg = current_task->rec_msg(state, pid);
    if(msg != nullptr){
        int16_t target = msg->pid;
        process_manager.process_mem_manager.free(msg);
        return target;
    } else {
        return -1;
    }
}


uint32_t buyaba[128];
uint32_t buya = 0;

void foo(){
    printf("foo\n");
    int8_t i[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    printf_int(Kernel::fork());
    printf(" ");

    printf_int(Kernel::fork());
    printf(" ");

    printf_int(Kernel::fork());
    printf(" ");

    printfHex32((uint32_t)i);
    printf(" ");

    for(int j = 0; j < 9; j++)
        printf_int(i[j]);
    printf(" foo again\n");
    while(1);
        // printf("1");
}

void foo2(void* param) {
    printf("foo2\n");
    // printf_int(*((int32_t*)param));

    // if(Kernel::fork() != 0)
        Kernel::fork();

    while(1);
        // printf("foo2 ");
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
    if(num == 1) {
        // printf(" finished\n");
        return;
    } else if(num % 2 == 0) {
        printf_int(num/2);
        printf(" ");
        collatz_sequence(num/2);
    } else {
        printf_int(num*3+1);
        printf(" ");
        collatz_sequence(num*3+1);
    }
    // printf("coming back\n");
    return;
}

struct search_param {
    int32_t* arr;
    size_t length;
    int16_t target;
};

void Kernel::dump_stack() {
    active_kernel->process_manager.dump_stack();
}

void task_collatz_sequence(void* param) {
    // for(int32_t i = (int32_t)param; i > 0; i--) {
        printf_int((int32_t)param);
        printf(": ");
        collatz_sequence((int32_t)param);
        // printf("finished a number\n");
    // }

    printf("finished\n");

    while(1);
}

void task_binary_search(void* param) {
    printf("binary search: ");
    printf_int(
        binary_search(
            ((struct search_param*)param)->arr, 
            ((struct search_param*)param)->length, 
            ((struct search_param*)param)->target
        )
    );
    printf("\n");

    Kernel::exit();

    // while(1);
}

void task_linear_search(void* param) {
    printf("linear search: ");
    printf_int(
        linear_search(
            ((struct search_param*)param)->arr, 
            ((struct search_param*)param)->length, 
            ((struct search_param*)param)->target
        )
    );
    printf("\n");

    Kernel::exit();

    // while(1);
}

void init() {
    char* buffer = "                      ";
    printf("init entered\n");
    int32_t i = 1453;

    if(Kernel::fork() == 0){
        i++;
        printfHex32((uint32_t) &i);
        printf(" ");
        printf_int(i);
        printf(" child\n");
        // Kernel::execve((char*)task_binary_search, (char *const *)&i, nullptr);
    } else {
        // i++;
        printfHex32((uint32_t) &i);
        printf(" ");
        printf_int(i);
        printf(" parent");
    }

    if(Kernel::fork() == 0){
        i++;
        printfHex32((uint32_t) &i);
        printf(" ");
        printf_int(i);
        printf(" child\n");
        // Kernel::execve((char*)task_binary_search, (char *const *)&i, nullptr);
    } else {
        // i++;
        printfHex32((uint32_t) &i);
        printf(" ");
        printf_int(i);
        printf(" parent");
    }

    printf(" init inside\n");

    while(1);
        // printf("init ");
}


int32_t bs_arr[] = {10, 20, 30, 50, 60, 80, 100, 110, 130, 170};
size_t bs_length = 10;
int16_t bs_target = 110;
struct search_param bs_param = {bs_arr, bs_length, bs_target};
struct search_param* bs_param_list[] = {&bs_param};

int32_t ls_arr[] = {10, 20, 80, 30, 60, 50, 110, 100, 130, 170};
size_t ls_length = 10;
int16_t ls_target = 175;
struct search_param ls_param = {ls_arr, ls_length, ls_target};

int32_t collatz_in = 7;

void Kernel::get_processes() {
    active_kernel->process_manager.print_processes();
}

void init1() {
    int16_t pid;

    if((pid = Kernel::fork()) == 0)
        Kernel::execve((char*)task_binary_search, (void*)&bs_param);
    else {
        printf("Binary search forked with pid ");
        printf_int(pid);
        printf("\n");
    }

    if((pid = Kernel::fork()) == 0)
        Kernel::execve((char*)task_linear_search, (void*)&ls_param);
    else {
        printf("Linear search forked with pid ");
        printf_int(pid);
        printf("\n");
    }

    // if((pid = Kernel::fork()) == 0)
    //     Kernel::execve((char*)collatz_sequence, (void*)collatz_in);
    // else {
    //     printf("Collatz Sequence forked with pid ");
    //     printf_int(pid);
    //     printf("\n");
    // }

    // Kernel::get_processes();

    while(1) {
        pid = Kernel::waitpid(-1, TERMINATED);
        if(pid != -1) {
            printf("Process exited with pid ");
            printf_int(pid);
            printf("\n");
        }
    }
}

void init2() {
    int16_t pid;
    for(int i = 0; i < 10; i++){
        if((pid = Kernel::fork()) == 0)
            Kernel::execve((char*)task_binary_search, (void*)&bs_param);
        else {
            printf("Binary search forked with pid ");
            printf_int(pid);
            printf("\n");
        }
    }

    while(1) {
        pid = Kernel::waitpid(-1, TERMINATED);
        if(pid != -1) {
            printf("Process exited with pid ");
            printf_int(pid);
            printf("\n");
        }
    }
}

void init3() {
    int16_t pid;
    for(int i = 0; i < 3; i++){
        if((pid = Kernel::fork()) == 0)
            Kernel::execve((char*)task_binary_search, (void*)&bs_param);
        else {
            printf("Binary search forked with pid ");
            printf_int(pid);
            printf("\n");
        }

        if((pid = Kernel::fork()) == 0)
            Kernel::execve((char*)task_linear_search, (void*)&ls_param);
        else {
            printf("Linear search forked with pid ");
            printf_int(pid);
            printf("\n");
        }
    }

    while(1) {
        pid = Kernel::waitpid(-1, TERMINATED);
        if(pid != -1) {
            printf("Process exited with pid ");
            printf_int(pid);
            printf("\n");
        }
    }
}


void Kernel::run() 
{
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
    
    interrupt_manager.activate();
    
    create_task((void*)init3, 4096);

    while(1);
}

uint16_t Kernel::fork()
{
    uint32_t result_child;
    asm("int $0x80" : "=b" (result_child) : "a" (2), "c" (__builtin_return_address(0)));
    // dump_stack();
    return result_child;
}

void Kernel::exit() {
    asm("int $0x80" : : "a" (1));
}

int16_t Kernel::waitpid(uint16_t pid, ProcessState status) {
    int16_t result;
    asm("int $0x80" : "=d" (result) : "a" (7), "b" (pid), "c" (status));
    return result;
}

int Kernel::execve(void* pathname, void* argv) {
    asm("int $0x80" : : "a" (11), "b" (pathname), "c" (argv));
}
