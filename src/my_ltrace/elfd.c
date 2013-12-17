#include <elf.h>
#include <gelf.h>
#include <libelf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "elfd.h"
#include "log.h"
#include "br.h"

static void handle_dynsym_section(elf_info_s *elf, Elf_Scn *scn, GElf_Shdr shdr)
{
    Elf_Data *data;
    /* get .dynsym data and size */
    elf->dynsym = elf_getdata(scn, NULL);
    elf->dynsym_count = shdr.sh_size / shdr.sh_entsize;

    /* retrieve the header table index link which point to .dynstr */
    scn = elf_getscn(elf->elf, shdr.sh_link);
    gelf_getshdr(scn, &shdr);
    /* get the data of .dynstr */
    data = elf_getdata(scn, NULL);
    elf->dynstr = data->d_buf;
}

static void handle_dynamic_section(elf_info_s *elf, Elf_Scn *scn, GElf_Shdr shdr)
{
    Elf_Data *data;
    GElf_Addr replt_addr;
    size_t replt_count;

    /* get data of .dynamic */
    data = elf_getdata(scn, NULL);

    /* iterate through .dynamic */
    for (size_t i = 0; i < shdr.sh_size / shdr.sh_entsize; ++i) {
        GElf_Dyn dyn;

        // get entries i
        gelf_getdyn(data, i, &dyn);
        // if replt
        if (dyn.d_tag == DT_JMPREL)
            replt_addr = dyn.d_un.d_ptr;
        // if replt_size
        if (dyn.d_tag == DT_PLTRELSZ)
            replt_count = dyn.d_un.d_val;
    }
    LOG(INFO, "Section relplt at 0x%lx (%zu)", replt_addr, replt_count);
    for (size_t i = 1; i < elf->hdr.e_shnum; ++i) {
        Elf_Scn *scn;
        GElf_Shdr shdr;
        scn = elf_getscn(elf->elf, i);
        gelf_getshdr(scn, &shdr);
        if (shdr.sh_addr == replt_addr && shdr.sh_size == replt_count) {
            elf->replt = elf_getdata(scn, NULL);
            elf->replt_count = shdr.sh_size / shdr.sh_entsize;
            break;
        }
    }
}

static void handle_plt_section(elf_info_s *elf, GElf_Shdr shdr)
{
    elf->plt_addr = shdr.sh_addr;
    elf->plt_count = shdr.sh_size;
}

static void handle_gotplt_section(elf_info_s *elf, GElf_Shdr shdr)
{
    elf->gotplt_addr = shdr.sh_addr;
    elf->gotplt_count = shdr.sh_size;
    LOG(INFO, "Section gotplt at 0x%lx (%zu)", shdr.sh_addr, shdr.sh_size);
}

static void find_symbols(elf_info_s *elf)
{
    for (size_t i = 0; i < elf->replt_count; ++i) {
        void *ret;
        GElf_Rel rel;
        GElf_Rela rela;
        const char *name;
        GElf_Sym sym;
        GElf_Addr addr;
        /* local relocation entries */
        if (elf->replt->d_type == ELF_T_REL) {
            ret = gelf_getrel(elf->replt, i, &rel);
            rela.r_offset = rel.r_offset;
            rela.r_info = rel.r_info;
            rela.r_addend = 0;
        }
        /* external relocation entries */
        else
            ret = gelf_getrela(elf->replt, i, &rela);

        gelf_getsym(elf->dynsym, ELF64_R_SYM(rela.r_info), &sym);

        name = elf->dynstr + sym.st_name;
        addr = elf->plt_addr + (i + 1) * 16;
        LOG(INFO, "ADD %s at 0x%lx", name, addr);
        breakpoint_create(addr, name);
    }
}

elf_info_s *elf_symbols(int fd)
{
    /* Open Elf */
    elf_version(EV_CURRENT);
    elf_info_s *elf = calloc(1, sizeof (elf_info_s));
    elf->elf = elf_begin(fd, ELF_C_READ, NULL);
    gelf_getehdr(elf->elf, &elf->hdr);

    /* Iterate through the sections */
    Elf_Scn *scn = NULL;
    while ((scn = elf_nextscn(elf->elf, scn)) != 0) {
        GElf_Shdr shdr;
        const char *name;
        /* get section header */
        gelf_getshdr(scn, &shdr);
        /* get section name */
        name = elf_strptr(elf->elf, elf->hdr.e_shstrndx, shdr.sh_name);
        //LOG(INFO, "Iter on %s", name);

        if (shdr.sh_type == SHT_DYNSYM)
            handle_dynsym_section(elf, scn, shdr);
        else if (shdr.sh_type == SHT_DYNAMIC)
            handle_dynamic_section(elf, scn, shdr);
        else if ((shdr.sh_type == SHT_PROGBITS || shdr.sh_type == SHT_NOBITS)
                && !strcmp(name, ".plt"))
            handle_plt_section(elf, shdr);
        else if (!strcmp(name, ".got.plt"))
            handle_gotplt_section(elf, shdr);
    }
    find_symbols(elf);
    return elf;
}
