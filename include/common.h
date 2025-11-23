#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

typedef uint32_t size_t;

typedef uint32_t paddr32_t;
typedef uint32_t vaddr32_t;
typedef uint64_t paddr64_t;
typedef uint64_t vaddr64_t;
typedef uint32_t bool;

typedef uint32_t uintptr_t;
typedef uint64_t uintptr64_t;

#define true 1
#define false 0
#define NULL ((void *)0)
#define alignup(x, align) __builtin_align_up((x), (align))
#define aligndown(x, align) __builtin_align_down((x), (align))
#define is_aligned(x, align) __builtin_is_aligned((x), (align))
#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

#define PAGE_SIZE 4096