#include <libelfin/dwarf/dwarf++.hh>
#include <libelfin/elf/elf++.hh>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s elf-file\n", argv[0]);
        return 2;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    elf::elf ef(elf::create_mmap_loader(fd));
    dwarf::dwarf dw(dwarf::elf::create_loader(ef));

    for (const auto &cu : dw.compilation_units()) {
        for (const auto &die : cu.root()) {
            if (die.tag == dwarf::DW_TAG::subprogram) {
                if (die.has(dwarf::DW_AT::name)) {
                    std::cout << "name = " << dwarf::at_name(die) << std::endl;
                }
            }
        }
    }

    return 0;
}