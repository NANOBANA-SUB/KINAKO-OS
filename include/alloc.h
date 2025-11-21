#pragma once
#include "common.h"

extern char __free_ram[];
extern char __free_ram_end[];

paddr32_t alloc_pages(size_t num_pages);
void free_pages(paddr32_t addr, size_t num_pages);