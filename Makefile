# ==== config ======================================================

ARCH       := riscv
CROSS_COMPILE ?= riscv64-unknown-elf-

CC      := clang
OBJCOPY := $(CROSS_COMPILE)objcopy
QEMU    := qemu-system-riscv32

BUILDDIR   := build
OBJDIR     := $(BUILDDIR)

KERNEL_ELF := $(BUILDDIR)/kernel.elf
KERNEL_BIN := $(BUILDDIR)/kernel.bin

ARCH_DIR    := arch/$(ARCH)
INCLUDE_ARCH_DIR := $(ARCH_DIR)/include
INCLUDE_DIR := include

TEST_DIR := test

LDSCRIPT := $(ARCH_DIR)/kernel/linker.ld

CFLAGS := -Wall -Wextra -O2 \
		  -g3 \
		  --target=$(ARCH)32-unknown-elf \
          -ffreestanding -nostdlib -nostartfiles \
          -fno-builtin -fno-stack-protector \
          -I$(INCLUDE_DIR) -I$(INCLUDE_ARCH_DIR)/asm

LDFLAGS := -nostdlib -nostartfiles -ffreestanding

# ==== sources / objects ===========================================

SRCS_ARCH := \
    $(wildcard $(ARCH_DIR)/kernel/*.c) \
    $(wildcard $(ARCH_DIR)/kernel/*.S)

SRCS_KERNEL := \
	$(wildcard kernel/*.c)

SRCS_LIB    := \
	$(wildcard lib/*.c)

SRCS_MM := \
	$(wildcard mm/*.c)

SRCS_MMTEST := \
	$(wildcard $(TEST_DIR)/mm/*.c)

SRCS_LIBTEST := \
	$(wildcard $(TEST_DIR)/lib/*.c)

SRCS := $(SRCS_ARCH) $(SRCS_KERNEL) $(SRCS_LIB) $(SRCS_MM)

TEST_SRCS := $(SRCS_LIBTEST) $(SRCS_MMTEST)

# src: arch/riscv/kernel/boot.c → obj: build/arch/riscv/kernel/boot.o
OBJS := $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
OBJS := $(OBJS:.S=.o)

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

# ライブラリテスト用ターゲット
.PHONY: libtest
libtest: $(TEST_SRCS:%.c=$(OBJDIR)/%.o)

# ライブラリテストのビルド
$(OBJDIR)/$(TEST_DIR)/lib/%.o: $(TEST_DIR)/lib/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


# ビルド成果物の削除
clean:
	rm -rf $(BUILDDIR)

# QEMUで実行0x80200000 にマップされる ELF をそのまま渡す想定）
run: $(KERNEL_ELF)
	$(QEMU) -machine virt -nographic -kernel $(KERNEL_ELF)

debug: $(KERNEL_ELF)
	$(QEMU) -machine virt -nographic -kernel $(KERNEL_ELF) -s -S