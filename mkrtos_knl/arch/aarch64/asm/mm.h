#pragma once

#define CONFIG_ARM64_VA_BITS 40

#define PAGE_SHIFT (CONFIG_PAGE_SHIFT)
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define VA_BITS (CONFIG_ARM64_VA_BITS)

#ifndef __ASSEMBLER__

void memzero(unsigned long src, unsigned long n);

#endif
