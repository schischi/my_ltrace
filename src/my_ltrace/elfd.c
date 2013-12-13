#include <elf.h>
#include <gelf.h>
#include <libelf.h>
#include <stdlib.h>
#include <stdio.h>
#include "elfd.h"
#include "log.h"

elf_info_s *elf_symbols(int fd)
{
    int err;
    if (elf_version(EV_CURRENT) == EV_NONE) {
        err = elf_errno();
        LOG(ERROR, "%s", elf_errmsg(err));
    }
    elf_info_s *elf = malloc(sizeof (elf_info_s));
    /* Open elf */
    elf->elf = elf_begin(fd, ELF_C_READ, NULL);
    if (elf->elf == NULL) {
        err = elf_errno();
        if (err != 0)
            LOG(ERROR, "%s", elf_errmsg(err));
        else
            LOG(ERROR, "Unable to open elf at %d", fd);
        return NULL;
    }
    /* Iterate through the sections */
    Elf_Scn *scn = NULL;
    GElf_Shdr shdr;
    while ((scn = elf_nextscn(elf->elf, scn)) != 0) {
        /* get section header */
        if (gelf_getshdr(scn, &shdr) == NULL)
            return NULL;
        /* Skip non-symtab */
        if (shdr.sh_type == SHT_SYMTAB)
            break;
    }
    elf->symtab = elf_getdata(scn, NULL);
    elf->symtab_size = shdr.sh_size / shdr.sh_entsize;

    for (size_t i = 0; i < elf->symtab_size; ++i) {
        GElf_Sym sym;
        gelf_getsym(elf->symtab, i, &sym);
        if ((sym.st_info & STT_FUNC) == STT_FUNC)
            printf("%s (%d)\n", elf_strptr(elf->elf, shdr.sh_link, sym.st_name), sym.st_info);
    }
    return NULL;
}
