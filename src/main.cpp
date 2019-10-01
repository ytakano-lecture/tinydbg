#include "linenoise.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <libelfin/dwarf/dwarf++.hh>
#include <libelfin/elf/elf++.hh>

// command:
//   registers
void do_registers(int pid) {}

// command:
//   continue
void do_continue(int pid) {}

// command:
//   step
void do_step(int pid) {
    // implement here
}

// command:
//   next
void do_next(int pid) {
    // implement here
}

// command:
//   finish
void do_finish(int pid) {
    // implement here
}

// command:
//   break FILE:LINE
// ex:
//   break main.c:10
//   then
//   do_delete_file(pid, "main.c", 10) is called
void do_delete_file(int pid, char *file, int line) {
    // implement here
}

// command:
//   break FUNCTION
// ex:
//   break main
//   then
//   do_delete_func(pid, "main") is called
void do_delete_func(int pid, char *func) {
    // implement here
}

// command:
//   break ADDRESS
// ex:
//   break 0x40000
//   then
//   do_delete_addr(pid, 0x4000) is called
void do_delete_addr(int pid, long addr) {
    // implement here
}

// command:
//   break FILE:LINE
// ex:
//   break main.c:10
//   then
//   do_break_file(pid, "main.c", 10) is called
void do_break_file(int pid, char *file, int line) {
    // implement here
}

// command:
//   break FUNCTION
// ex:
//   break main
//   then
//   do_break_func(pid, "main") is called
void do_break_func(int pid, char *func) {
    // implement here
}

// command:
//   break ADDRESS
// ex:
//   break 0x40000
//   then
//   do_break_addr(pid, 0x4000) is called
void do_break_addr(int pid, long addr) {
    // implement here
}

char *get_arg() {
    for (;;) {
        char *p = strtok(NULL, " ");
        if (p == NULL)
            return NULL;

        if (strlen(p) != 0)
            return p;
    }

    return NULL;
}

void parse_arg(long *addr, char **left, char **right) {
    char *arg = get_arg();

    if (strlen(arg) > 1 && arg[0] == '0' && arg[1] == 'x') {
        *addr = strtol(arg, NULL, 16);
        *left = NULL;
        *right = NULL;
    } else {
        *left = strtok(arg, ":");
        *right = strtok(NULL, " ");
    }
}

void do_command(int pid, char *line) {
    char *cmd = strtok(line, " ");
    if (cmd == NULL)
        goto err;

    if (strcmp(cmd, "continue") == 0) {
        do_continue(pid);
    } else if (strcmp(cmd, "step") == 0) {
        do_step(pid);
    } else if (strcmp(cmd, "next") == 0) {
        do_next(pid);
    } else if (strcmp(cmd, "finish") == 0) {
        do_finish(pid);
    } else if (strcmp(cmd, "registers") == 0) {
        do_registers(pid);
    } else if (strcmp(cmd, "delete") == 0) {
        char *left, *right;
        long addr;
        parse_arg(&addr, &left, &right);
        if (right == NULL) {
            if (left == NULL) {
                do_delete_addr(pid, addr);
            } else {
                do_delete_func(pid, left);
            }
        } else {
            do_delete_file(pid, left, atoi(right));
        }
    } else if (strcmp(cmd, "break") == 0) {
        char *left, *right;
        long addr;
        parse_arg(&addr, &left, &right);
        if (right == NULL) {
            if (left == NULL) {
                do_break_addr(pid, addr);
            } else {
                do_break_func(pid, left);
            }
        } else {
            do_break_file(pid, left, atoi(right));
        }
    } else {
    err:
        printf("command:\n"
               "  continue\n"
               "  step\n"
               "  next\n"
               "  finish\n"
               "  registers\n"
               "  break\n"
               "  delete\n");
    }
}

void dbg_loop(int pid) {
    int status;
    waitpid(pid, &status, 0);

    char *line = NULL;
    while ((line = linenoise("tinydbg> ")) != NULL) {
        do_command(pid, line);
        linenoiseHistoryAdd(line);
        linenoiseFree(line);
    }
}

void completion(const char *buf, linenoiseCompletions *lc) {
    switch (buf[0]) {
    case 'c':
        linenoiseAddCompletion(lc, "continue");
        break;
    case 's':
        linenoiseAddCompletion(lc, "step");
        break;
    case 'n':
        linenoiseAddCompletion(lc, "next");
        break;
    case 'b':
        linenoiseAddCompletion(lc, "break ");
        break;
    case 'd':
        linenoiseAddCompletion(lc, "delete ");
        break;
    case 'f':
        linenoiseAddCompletion(lc, "finish");
    case 'r':
        linenoiseAddCompletion(lc, "registers");
        break;
    default:;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr,
                "too few arguments.\n"
                "usage: %s file\n",
                argv[0]);
        return 1;
    }

    int pid = fork();
    if (pid == 0) {
        // child process
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        if (execl(argv[1], argv[1], NULL) == -1) {
            perror("failed execl");
            return 1;
        }
    } else {
        auto fd = open(argv[1], O_RDONLY);

        auto elfinfo = elf::elf{elf::create_mmap_loader(fd)};
        auto dwarfinfo = dwarf::dwarf{dwarf::elf::create_loader(elfinfo)};

        // parent process
        linenoiseSetCompletionCallback(completion);
        dbg_loop(pid);
    }

    return 0;
}