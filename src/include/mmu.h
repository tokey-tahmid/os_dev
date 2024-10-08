/**
 * @file mmu.h
 * @author Stephen Marz (sgm@utk.edu)
 * @brief Memory Management Unit
 * @version 0.1
 * @date 2022-05-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

#define MMU_LEVEL_1G        2
#define MMU_LEVEL_2M        1
#define MMU_LEVEL_4K        0

#define PAGE_SIZE_4K        PAGE_SIZE_AT_LVL(MMU_LEVEL_4K)
#define PAGE_SIZE_2M        PAGE_SIZE_AT_LVL(MMU_LEVEL_2M)
#define PAGE_SIZE_1G        PAGE_SIZE_AT_LVL(MMU_LEVEL_1G)

#define PAGE_SIZE_AT_LVL(x) (1 << (x * 9 + 12))

#define PB_NONE             0
#define PB_VALID            (1UL << 0)
#define PB_READ             (1UL << 1)
#define PB_WRITE            (1UL << 2)
#define PB_EXECUTE          (1UL << 3)
#define PB_USER             (1UL << 4)
#define PB_GLOBAL           (1UL << 5)
#define PB_ACCESS           (1UL << 6)
#define PB_DIRTY            (1UL << 7)

#define PB_USER_BOOL(x)     ((uint64_t)(!(x)) << 4)

#define SATP_MODE_BIT       60
#define SATP_MODE_SV39      (8UL << SATP_MODE_BIT)
#define SATP_ASID_BIT       44
#define SATP_PPN_BIT        0
#define SATP_SET_PPN(x)     ((((uint64_t)(x)) >> 12) & 0xFFFFFFFFFFFUL)
#define SATP_SET_ASID(x)    ((((uint64_t)(x)) & 0xFFFF) << SATP_ASID_BIT)

// The kernel's address space identifier is 0b1111_1111_1111_1111
#define KERNEL_ASID 0xFFFFUL

// Helpers to quickly make an SATP register value
#define SATP(table, asid) (SATP_MODE_SV39 | SATP_SET_PPN(table) | SATP_SET_ASID(asid))
#define SATP_KERNEL       SATP(kernel_mmu_table, KERNEL_ASID)

typedef struct PageTable {
    uint64_t entries[PAGE_SIZE_4K / sizeof(uint64_t)];
} PageTable;

#define MMU_TRANSLATE_PAGE_FAULT  (-1UL)

// Declared in src/main.c
extern PageTable *kernel_mmu_table;

PageTable *mmu_table_create(void);

uint64_t mmu_map_range(PageTable *tab, 
                            uint64_t start_virt, 
                            uint64_t end_virt, 
                            uint64_t start_phys,
                            unsigned char lvl, 
                            uint64_t bits);

uintptr_t mmu_translate(const PageTable *tab, 
                        uintptr_t vaddr);

uintptr_t kernel_mmu_translate(uintptr_t vaddr);

bool mmu_map(PageTable *tab, 
             uintptr_t vaddr, 
             uintptr_t paddr, 
             unsigned char lvl, 
             uintptr_t bits);

void mmu_free(PageTable *tab);

void debug_page_table(PageTable *tab, uint8_t lvl);

void mmu_print_entries(PageTable *tab, uint8_t lvl);