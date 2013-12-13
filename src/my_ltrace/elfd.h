#ifndef ELFD_H
# define ELFD_H

# include <elf.h>
# include <gelf.h>
# include <libelf.h>

struct elf_info {
    Elf *elf;
    GElf_Ehdr hdr;
    Elf_Data *symtab;
    size_t symtab_size;
    char *strtab;
};
typedef struct elf_info elf_info_s;

elf_info_s *elf_symbols(int fd);

#endif /* !ELFD_H */
