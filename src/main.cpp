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

void do_break_addr(int pid, long addr) {
    // implement here
}

void do_continue(int pid) {
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

void do_command(int pid, char *line) {
    char *cmd = strtok(line, " ");
    if (strcmp(cmd, "continue") == 0) {
        do_continue(pid);
    } else if (strcmp(cmd, "break") == 0) {
        char *arg = get_arg();
        if (strlen(arg) < 3) {
            printf("invalid argument\n");
            return;
        }
        if (!(arg[0] == '0' && arg[1] == 'x')) {
            long addr = strtol(arg, NULL, 16);
            do_break_addr(pid, addr);
            return;
        } else {
            printf("invalid argument\n");
        }
    } else {
        printf("command:\n"
               "  continue\n"
               "  step\n"
               "  next\n"
               "  finish\n"
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
        linenoiseAddCompletion(lc, "delete");
        break;
    case 'f':
        linenoiseAddCompletion(lc, "finish");
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