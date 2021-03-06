#ifndef ELFD_H
# define ELFD_H

# include <elf.h>
# include <gelf.h>
# include <libelf.h>
# include <sys/types.h>
# include "proc.h"
# include "map.h"

struct elf_info {
    Elf *elf;
    GElf_Ehdr hdr;

    GElf_Addr plt_addr;
    size_t plt_count;

    Elf_Data *replt;
    size_t replt_count;

    GElf_Addr gotplt_addr;
    size_t gotplt_count;

    Elf_Data *dynsym;
    size_t dynsym_count;

    const char *dynstr;
};
typedef struct elf_info elf_info_s;

map_s *elf_set_breakpoints(proc_s *proc);

#endif /* !ELFD_H */
