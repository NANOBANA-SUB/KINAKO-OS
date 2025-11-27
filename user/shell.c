#include "user.h"

extern char _binary_build_user_shell_bin_start[];
extern char _binary_build_user_shell_bin_size[];

void main(void)
{
    uint8_t *shell_bin = (uint8_t *) _binary_build_user_shell_bin_start;
}