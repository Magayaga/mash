// time.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s command [arguments...]\n", progname);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    struct timeval start_real, end_real;
    struct rusage start_cpu, end_cpu;

    gettimeofday(&start_real, NULL);
    getrusage(RUSAGE_CHILDREN, &start_cpu);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // Child process: run the command
        execvp(argv[1], &argv[1]);
        perror("execvp failed");
        exit(127);
    } else {
        // Parent process: wait for child
        int status;
        waitpid(pid, &status, 0);

        gettimeofday(&end_real, NULL);
        getrusage(RUSAGE_CHILDREN, &end_cpu);

        // Time calculations
        double real_sec = (end_real.tv_sec - start_real.tv_sec) +
                          (end_real.tv_usec - start_real.tv_usec) / 1e6;

        double user_sec = (end_cpu.ru_utime.tv_sec - start_cpu.ru_utime.tv_sec) +
                          (end_cpu.ru_utime.tv_usec - start_cpu.ru_utime.tv_usec) / 1e6;

        double sys_sec = (end_cpu.ru_stime.tv_sec - start_cpu.ru_stime.tv_sec) +
                         (end_cpu.ru_stime.tv_usec - start_cpu.ru_stime.tv_usec) / 1e6;

        fprintf(stderr, "\n");
        fprintf(stderr, "real\t%.3f sec\n", real_sec);
        fprintf(stderr, "user\t%.3f sec\n", user_sec);
        fprintf(stderr, "sys \t%.3f sec\n", sys_sec);

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return 1;
        }
    }
}
