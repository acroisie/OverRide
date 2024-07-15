#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>

int main() {
    pid_t pid;
    int status;
    long ptrace_ret;
    char buffer[128] = {0};

    pid = fork();

    if (pid == 0) {  // Child process
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        puts("Give me some shellcode, k");
        gets(buffer);
    } else {  // Parent process
        wait(&status);

        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            puts("child is exiting...");
            return 0;
        }

        while(1) {
            ptrace_ret = ptrace(PTRACE_PEEKUSER, pid, 4 * ORIG_EAX, NULL);
            
            if (ptrace_ret == 11) {  // 11 is the syscall number for execve
                puts("no exec() for you");
                kill(pid, SIGKILL);
                break;
            }

            if (waitpid(pid, &status, 0) && (WIFEXITED(status) || WIFSIGNALED(status))) {
                break;
            }
        }
    }

    return 0;
}