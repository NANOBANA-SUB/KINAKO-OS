#include "user.h"

extern char _binary_build_user_shell_bin_start[];
extern char _binary_build_user_shell_bin_size[];

void main(void)
{
    // while (1) { }  // ここでブレークする
    puts("Hello world!\n");
}