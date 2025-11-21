# ==== config ======================================================

ARCH       := riscv
CROSS_COMPILE ?= riscv64-unknown-elf-

CC      := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
QEMU    := qemu-system-riscv32

BUILDDIR   := build
OBJDIR     := $(BUILDDIR)

KERNEL_ELF := $(BUILDDIR)/kernel.elf
KERNEL_BIN := $(BUILDDIR)/kernel.bin

ARCH_DIR    := arch/$(ARCH)
INCLUDE_ARCH_DIR := $(ARCH_DIR)/include
INCLUDE_DIR := include

LDSCRIPT := $(ARCH_DIR)/kernel/linker.ld

CFLAGS := -Wall -Wextra -O2 \
          -ffreestanding -nostdlib -nostartfiles \
          -fno-builtin -fno-stack-protector \
          -march=rv32imac -mabi=ilp32 \
          -I$(INCLUDE_DIR) -I$(INCLUDE_ARCH_DIR)/asm

LDFLAGS := -nostdlib -nostartfiles -ffreestanding

# ==== sources / objects ===========================================

SRCS_ARCH   := \
	$(wildcard $(ARCH_DIR)/kernel/*.c)

SRCS_KERNEL := \
	$(wildcard kernel/*.c)

SRCS_LIB    := \
	$(wildcard lib/*.c)

SRCS := $(SRCS_ARCH) $(SRCS_KERNEL) $(SRCS_LIB)

# src: arch/riscv/kernel/boot.c → obj: build/arch/riscv/kernel/boot.o
OBJS := $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))

# ==== rules =======================================================

.PHONY: all clean run

all: $(KERNEL_ELF) $(KERNEL_BIN)

# ELFをリンク
$(KERNEL_ELF): $(OBJS) $(LDSCRIPT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-T,$(LDSCRIPT) -o $@ $(OBJS)

# 生バイナリに変換
$(KERNEL_BIN): $(KERNEL_ELF)
	@mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $< $@

# Cファイルのコンパイル
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# アセンブリファイルのコンパイル
$(OBJDIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

# QEMUで実行0x80200000 にマップされる ELF をそのまま渡す想定）
run: $(KERNEL_ELF)
	$(QEMU) -machine virt -nographic -kernel $(KERNEL_ELF)