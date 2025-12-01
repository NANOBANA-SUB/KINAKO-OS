#pragma once
#include "common.h"

__attribute__((noreturn)) void exit(void);
void putchar(char ch);
void puts(const char *buf);
int getchar(void);
int open(const char *path, int omode);
int read(int fd, void *buf, int n);
int close(int fd);
int write(int fd, const void *buf, int n);