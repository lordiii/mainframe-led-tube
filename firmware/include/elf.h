#ifndef ELF_H
#define ELF_H

#include <Arduino.h>

#define EI_NIDENT    16

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;
typedef uint16_t Elf32_Versym;

typedef struct elf32_hdr {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;  /* Entry point */
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

/* e_ident[] indexes */
#define    EI_MAG0        0
#define    EI_MAG1        1
#define    EI_MAG2        2
#define    EI_MAG3        3
#define    EI_CLASS    4
#define    EI_DATA        5
#define    EI_VERSION    6
#define    EI_OSABI    7
#define    EI_PAD        8

#define    ELFMAG0        0x7F
#define    ELFMAG1        'E'
#define    ELFMAG2        'L'
#define    ELFMAG3        'F'

/* EI_CLASS */
#define    ELFCLASSNONE    0
#define    ELFCLASS32    1
#define    ELFCLASS64    2
#define    ELFCLASSNUM    3

/* EI_DATA */
#define ELFDATANONE    0
#define ELFDATA2LSB    1
#define ELFDATA2MSB    2

/* EI_VERSION */
#define EV_NONE        0
#define EV_CURRENT    1
#define EV_NUM        2

/* EI_OSABI */
#define ELFOSABI_NONE    0

/* These constants define the different elf file types */
#define ET_NONE   0
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3
#define ET_CORE   4

#endif