# ==== basic config =================================================

ARCH   := riscv
TARGET := riscv32-unknown-elf   # clang の --target に渡す

CC      := clang
OBJCOPY := /opt/homebrew/opt/llvm/bin/llvm-objcopy
QEMU    := qemu-system-riscv32

BUILDDIR := build
OBJDIR   := $(BUILDDIR)

# カーネル成果物
KERNEL_ELF := $(BUILDDIR)/kernel.elf
KERNEL_BIN := $(BUILDDIR)/kernel.bin

# パス類
ARCH_DIR         := arch/$(ARCH)
INCLUDE_DIR      := include
INCLUDE_ARCH_DIR := $(ARCH_DIR)/include

# ユーザプログラム
USER_DIR         := user
INCLUDE_USER_DIR := $(USER_DIR)/include

# リンカスクリプト
LDSCRIPT      := $(ARCH_DIR)/kernel/linker.ld
USER_LDSCRIPT := $(INCLUDE_USER_DIR)/user.ld

# シェル埋め込み用
SHELL_ELF     := $(BUILDDIR)/user/shell.elf
SHELL_BIN     := $(BUILDDIR)/user/shell.bin
SHELL_BIN_OBJ := $(BUILDDIR)/user/shell.bin.o   # これをカーネルにリンクする

# ==== flags ========================================================

CFLAGS_COMMON := \
	-Wall -Wextra -O2 \
	-g3 \
	-ffreestanding -nostdlib -nostartfiles \
	-fno-builtin -fno-stack-protector \
	--target=$(TARGET)

# カーネル用コンパイルフラグ
CFLAGS_KERNEL := $(CFLAGS_COMMON) \
	-I$(INCLUDE_DIR) \
	-I$(INCLUDE_ARCH_DIR)/asm

# ユーザ用コンパイルフラグ
CFLAGS_USER := $(CFLAGS_COMMON) \
	-I$(INCLUDE_DIR) \
	-I$(INCLUDE_USER_DIR)

# リンクフラグ（カーネル/ユーザ共通）
LDFLAGS := \
	--target=$(TARGET) \
	-ffreestanding -nostdlib -nostartfiles \
	-fuse-ld=lld

# ==== sources / objects ===========================================

# カーネル側ソース
SRCS_ARCH := \
	$(wildcard $(ARCH_DIR)/kernel/*.c) \
	$(wildcard $(ARCH_DIR)/kernel/*.S)

SRCS_KERNEL := \
	$(wildcard kernel/*.c)

SRCS_LIB := \
	$(wildcard lib/*.c)

SRCS_MM := \
	$(wildcard mm/*.c)

KERNEL_SRCS := $(SRCS_ARCH) $(SRCS_KERNEL) $(SRCS_LIB) $(SRCS_MM)

# build/ を頭に付けた .o に変換
KERNEL_OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(KERNEL_SRCS))
KERNEL_OBJS := $(patsubst %.S,$(OBJDIR)/%.o,$(KERNEL_OBJS))

# ユーザ側ソース
SRCS_USER := $(wildcard $(USER_DIR)/*.c)
USER_OBJS := $(patsubst $(USER_DIR)/%.c,$(OBJDIR)/$(USER_DIR)/%.o,$(SRCS_USER))

# ==== phony targets ===============================================

.PHONY: all clean run debug

all: $(KERNEL_ELF) $(KERNEL_BIN)

# ==== kernel build =================================================
# カーネル ELF は「カーネルの .o + shell.bin.o」でリンクする

$(KERNEL_ELF): $(KERNEL_OBJS) $(SHELL_BIN_OBJ) $(LDSCRIPT)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-T,$(LDSCRIPT) -Wl,-Map,$(BUILDDIR)/kernel.map \
		-o $@ $(KERNEL_OBJS) $(SHELL_BIN_OBJ)

$(KERNEL_BIN): $(KERNEL_ELF)
	@mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $< $@

# ---- kernel object rules ----

# arch/riscv/kernel/*.c → build/arch/riscv/kernel/*.o
$(OBJDIR)/$(ARCH_DIR)/kernel/%.o: $(ARCH_DIR)/kernel/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_KERNEL) -c $< -o $@

# arch/riscv/kernel/*.S → build/arch/riscv/kernel/*.o
$(OBJDIR)/$(ARCH_DIR)/kernel/%.o: $(ARCH_DIR)/kernel/%.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_KERNEL) -c $< -o $@

# kernel/*.c → build/kernel/*.o
$(OBJDIR)/kernel/%.o: kernel/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_KERNEL) -c $< -o $@

# lib/*.c → build/lib/*.o
$(OBJDIR)/lib/%.o: lib/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_KERNEL) -c $< -o $@

# mm/*.c → build/mm/*.o
$(OBJDIR)/mm/%.o: mm/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_KERNEL) -c $< -o $@

# ==== user shell build ============================================

# 1. シェル ELF を作る（ユーザ専用リンカスクリプト user.ld 使用）
$(SHELL_ELF): $(USER_OBJS) $(USER_LDSCRIPT)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -Wl,-T,$(USER_LDSCRIPT) -Wl,-Map,$(BUILDDIR)/user/shell.map \
		-o $@ $(USER_OBJS)

# 2. ELF → 生バイナリ (shell.bin)
$(SHELL_BIN): $(SHELL_ELF)
	@mkdir -p $(dir $@)
	$(OBJCOPY) --set-section-flags .bss=alloc,contents -O binary $< $@

# 3. 生バイナリ → RISC-V ELF オブジェクト (shell.bin.o)
$(SHELL_BIN_OBJ): $(SHELL_BIN)
	@mkdir -p $(dir $@)
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@

# user/*.c → build/user/*.o
$(OBJDIR)/$(USER_DIR)/%.o: $(USER_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_USER) -c $< -o $@

# ==== misc ========================================================

clean:
	rm -rf $(BUILDDIR)

run: $(KERNEL_ELF)
	$(QEMU) -machine virt -nographic -kernel $(KERNEL_ELF)

debug: $(KERNEL_ELF)
	$(QEMU) -machine virt -nographic -kernel $(KERNEL_ELF) -s -S