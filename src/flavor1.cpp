#include "kernel.h"
#include "common/tests.h"

namespace microkernel
{
    extern "C" void init() {
        int16_t pid;

        if((pid = Kernel::fork()) == 0)
            Kernel::execve(task_binary_search, nullptr);
        else {
            printf("Binary search forked with pid ");
            printf_int(pid);
            printf("\n");
        }

        if((pid = Kernel::fork()) == 0)
            Kernel::execve(task_linear_search, nullptr);
        else {
            printf("Linear search forked with pid ");
            printf_int(pid);
            printf("\n");
        }

        if((pid = Kernel::fork()) == 0)
            Kernel::execve(task_collatz_sequence, nullptr);
        else {
            printf("Collatz Sequence forked with pid ");
            printf_int(pid);
            printf("\n");
        }

        Kernel::get_processes();

        while(1) {
            pid = Kernel::waitpid(-1, TERMINATED);
            if(pid != -1) {
                printf("Process exited with pid ");
                printf_int(pid);
                printf("\n");
            }
        }
    }
} // namespace microkernel
