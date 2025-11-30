#include "printk.h"
#include "string.h"

static int ktest_failures = 0;      // テスト失敗回数

#define KTEST_ASSERT(expr)                                       \
    do {                                                         \
        if (!(expr)) {                                           \
            printk("[KTEST] %s:%d: ASSERT FAILED: %s\n",         \
                   __FILE__, __LINE__, #expr);                   \
            ktest_failures++;                                    \
        }                                                        \
    } while (0)                                    \

static int sign(int x)
{
    if (x < 0) return -1;
    if (x > 0) return 1;
    return 0;
}


static void test_strncmp_basic(void)
{
    // n = 0
    KTEST_ASSERT(strncmp("abc", "def", 0) == 0);

    // 完全一致
    KTEST_ASSERT(strncmp("abc", "abc", 3) == 0);
    KTEST_ASSERT(strncmp("abc", "abc", 5) == 0);

    // 今回のバグパターン: 短い +大きなn
    KTEST_ASSERT(strncmp("a", "a", 3) == 0);
    KTEST_ASSERT(strncmp("", "", 5) == 0);

    // 長さ違い
    KTEST_ASSERT(sign(strncmp("ab", "abc", 3)) == -1);
    KTEST_ASSERT(sign(strncmp("abc", "ab", 3)) == 1);

    // 途中で不一致
    KTEST_ASSERT(sign(strncmp("abc", "abd", 3)) == -1);
    KTEST_ASSERT(strncmp("abc", "abd", 2) == 0);

    // 一方が空文字
    KTEST_ASSERT(sign(strncmp("a", "", 1)) == 1);
    KTEST_ASSERT(sign(strncmp("", "a", 1)) == -1);
}

void ktests_run_string(void)
{
    printk("[KTEST] string tests start\n");

    test_strncmp_basic();
    // ここに今後 strlen/memcpy のテストも足す

    if (ktest_failures == 0) {
        printk("[KTEST] string tests PASSED\n");
    } 
    else 
    {
        printk("[KTEST] string tests FAILED: %d failures\n", ktest_failures);
        // panicには現状しない
    }
}