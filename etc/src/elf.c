#include "etc/elf.h"

//! little-endian
#define ELF_MAGIC {0x7f, 'E', 'L', 'F'}

#define INSTRUCTION_SET_ARM 0x28

#define LITTLE_ENDIAN       0x01

#define WORD_SIZE_32        0x01

typedef struct elf_header_t {
    uint32_t       magic;
    uint8_t        word_size;
    uint8_t        endianness;
    uint8_t        header_version;
    uint8_t        abi;
    uint32_t       padding[2];
    uint16_t       type;
    uint16_t       instruction_set;
    uint32_t       elf_version;
    uint32_t       entry_offset;
    uint32_t       program_header_offset;
    uint32_t       section_header_offset;
    uint32_t       flags;
    uint16_t       header_size;
    uint16_t       program_header_entry_size;
    uint16_t       program_header_entries;
    uint16_t       section_header_entry_size;
    uint16_t       section_header_entries;
    uint16_t       string_section_header_idx;
} __attribute__((packed)) elf_header;

typedef struct program_header_t {
    uint32_t       type;
    uint32_t       p_offset;
    uint32_t       p_vaddr;
    uint32_t       p_paddr;
    uint32_t       p_filesz;
    uint32_t       p_memsz;
    uint32_t       flags;
    uint32_t       alignment;
} __attribute__((packed)) program_header;

typedef enum segment_type_t {
    SEG_NULL        = 0x00,
    SEG_LOAD        = 0x01,
    SEG_DYNAMIC     = 0x02,
    SEG_INTERP      = 0x03,
    SEG_NOTE        = 0x04,
} segment_type;

typedef enum segment_flags_t {
    FLAG_EXECUTABLE = (1 << 0),
    FLAG_WRITABLE   = (1 << 1),
    FLAG_READABLE   = (1 << 2),
} segment_flags;
