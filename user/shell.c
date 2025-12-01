#include "user.h"
#include "string.h"

extern char _binary_build_user_shell_bin_start[];
extern char _binary_build_user_shell_bin_size[];
extern int cat();

static int readline(char *buf, int buf_size)
{
    int len = 0;

    while (1) {
        int ch = getchar();

        if (ch == '\r' || ch == '\n') 
        {
            // Enter キー
            putchar('\n');
            buf[len] = '\0';
            return len;
        }

        // ここで Backspace / Delete を特別扱いする
        if (ch == '\b' || ch == 127) 
        {   // 0x08 or 0x7f
            if (len > 0) 
            {
                // 入力バッファから1文字消す
                len--;

                // 画面上の1文字を消す: 「1マス戻る → 空白で上書き → もう一回戻る」
                putchar('\b');
                putchar(' ');
                putchar('\b');
            }
            // 先頭で Backspace 押されたら何もしない
            continue;
        }

        // それ以外の普通の文字
        if (len < buf_size - 1) 
        {
            buf[len++] = (char)ch;
            putchar(ch);   // エコーバック
        }
        // いっぱいになったらそれ以上は無視
    }
}

void main(void)
{
    char line[128];

    while(1)
    {
        puts("> ");

        int n = readline(line, sizeof(line));
        if (n <= 0) 
        {
            continue;
        }

        if (!strcmp(line, "hello")) 
        {
            puts("Hello world from shell!\n");
        } 
        else if (!strcmp(line, "exit")) 
        {
            puts("Bye!\n");
            break;
        } 
        else if (!strcmp(line, "kinako"))
        {
            puts("Nanobana Kinako is very cute !\n");
        }
        else if (!strcmp(line, "cat"))
        {
            cat();
        }
        else
        {
            puts("unknown command: ");
            puts(line);
            puts("\n");
        }
    }
}