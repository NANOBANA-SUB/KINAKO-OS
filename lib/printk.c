#include "printk.h"

static void kprint_repeat(char ch, int n) 
{
    while (n-- > 0) console_putc(ch);
}

static int u64_dec_digits(uint64_t v) 
{
    int d = 1;
    while (v >= 10) { v /= 10; d++; }
    return d;
}

static void kprint_u64_dec(uint64_t v) 
{
    /* 逆順バッファに詰めてから出力（再帰なし） */
    char buf[21]; /* 2^64-1 は最大20桁 */
    int i = 0;
    do {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    } while (v);
    while (i--) console_putc(buf[i]);
}

static void kprint_i64_dec(long long v) 
{
    if (v < 0) { console_putc('-'); kprint_u64_dec((uint64_t)(-v)); }
    else kprint_u64_dec((uint64_t)v);
}

static void kprint_u64_hex(uint64_t v, int width, int zero_pad) {
    /* 幅指定が無いときは必要桁だけ、あるときはその幅に合わせる */
    int nibbles = 1;
    uint64_t tmp = v;
    while ((tmp >>= 4) != 0) nibbles++;
    if (width < nibbles) width = nibbles;
    if (zero_pad) kprint_repeat('0', width - nibbles);
    else          kprint_repeat(' ', width - nibbles);

    for (int i = width - 1; i >= 0; --i) {
        int shift = i * 4;
        uint8_t nib = (v >> shift) & 0xF;
        console_putc("0123456789abcdef"[nib]);
    }
}

/* 幅と'0'パディングだけ解釈する簡易パーサ */
static void parse_width(const char **pfmt, int *width, int *zero_pad) {
    const char *fmt = *pfmt;
    *width = 0; *zero_pad = 0;
    if (*fmt == '0') { *zero_pad = 1; fmt++; }
    while (*fmt >= '0' && *fmt <= '9') {
        *width = (*width * 10) + (*fmt - '0');
        fmt++;
    }
    *pfmt = fmt;
}

void printk(const char *fmt, ...)
{
    va_list vargs;
    va_start(vargs, fmt);

    while (*fmt) {
        if (*fmt != '%') { console_putc(*fmt++); continue; }

        fmt++; /* skip '%' */
        if (*fmt == '%') { console_putc('%'); fmt++; continue; }

        /* フラグ/幅/長さ修飾の簡易対応 */
        int width = 0, zero_pad = 0;
        parse_width(&fmt, &width, &zero_pad);

        /* 長さ修飾: なし / 'l' / 'll' のみ対応 */
        int len_l = 0; /* 0:なし, 1:l, 2:ll */
        if (*fmt == 'l') {
            len_l = 1; fmt++;
            if (*fmt == 'l') { len_l = 2; fmt++; }
        }

        char sp = *fmt ? *fmt++ : '\0';
        switch (sp) {
        case '\0':
            console_putc('%'); /* 末尾の不完全指定子は%だけ出す */
            goto end;

        case 'c': {
            int c = va_arg(vargs, int);
            console_putc((char)c);
            break;
        }

        case 's': {
            const char *s = va_arg(vargs, const char *);
            if (!s) s = "(null)";
            /* 幅指定が来ても左寄せ/右寄せは実装しない(必要なら追加) */
            while (*s) console_putc(*s++);
            break;
        }

        case 'd': /* signed decimal */
        case 'i': {
            if (len_l == 2) {
                long long v = va_arg(vargs, long long);
                /* 幅・ゼロパディング対応 */
                /* 桁数を先に計算してパディング */
                int neg = v < 0;
                uint64_t mag = neg ? (uint64_t)(-v) : (uint64_t)v;
                int digits = u64_dec_digits(mag);
                int pad = (width > digits + neg) ? (width - digits - neg) : 0;
                if (!zero_pad) kprint_repeat(' ', pad);
                if (neg) console_putc('-');
                if (zero_pad) kprint_repeat('0', pad);
                kprint_u64_dec(mag);
            } else if (len_l == 1) {
                long v = va_arg(vargs, long);
                int neg = v < 0;
                uint64_t mag = neg ? (uint64_t)(-v) : (uint64_t)v;
                int digits = u64_dec_digits(mag);
                int pad = (width > digits + neg) ? (width - digits - neg) : 0;
                if (!zero_pad) kprint_repeat(' ', pad);
                if (neg) console_putc('-');
                if (zero_pad) kprint_repeat('0', pad);
                kprint_u64_dec(mag);
            } else {
                int v = va_arg(vargs, int);
                int neg = v < 0;
                unsigned mag = neg ? (unsigned)(-v) : (unsigned)v;
                /* 32bitでも簡易パディング対応 */
                char buf[11]; int i = 0;
                do { buf[i++] = '0' + (mag % 10); mag /= 10; } while (mag);
                int digits = i;
                int pad = (width > digits + neg) ? (width - digits - neg) : 0;
                if (!zero_pad) kprint_repeat(' ', pad);
                if (neg) console_putc('-');
                if (zero_pad) kprint_repeat('0', pad);
                while (i--) console_putc(buf[i]);
            }
            break;
        }

        case 'u': { /* unsigned decimal */
            if (len_l == 2) {
                unsigned long long v = va_arg(vargs, unsigned long long);
                int digits = u64_dec_digits((uint64_t)v);
                int pad = (width > digits) ? (width - digits) : 0;
                if (!zero_pad) kprint_repeat(' ', pad);
                if (zero_pad)  kprint_repeat('0', pad);
                kprint_u64_dec((uint64_t)v);
            } else if (len_l == 1) {
                unsigned long v = va_arg(vargs, unsigned long);
                int digits = u64_dec_digits((uint64_t)v);
                int pad = (width > digits) ? (width - digits) : 0;
                if (!zero_pad) kprint_repeat(' ', pad);
                if (zero_pad)  kprint_repeat('0', pad);
                kprint_u64_dec((uint64_t)v);
            } else {
                unsigned v = va_arg(vargs, unsigned);
                /* 32bit簡易 */
                char buf[11]; int i = 0;
                do { buf[i++] = '0' + (v % 10); v /= 10; } while (v);
                int digits = i;
                int pad = (width > digits) ? (width - digits) : 0;
                if (!zero_pad) kprint_repeat(' ', pad);
                if (zero_pad)  kprint_repeat('0', pad);
                while (i--) console_putc(buf[i]);
            }
            break;
        }

        case 'x': { /* hex (lower) */
            if (len_l == 2) {
                unsigned long long v = va_arg(vargs, unsigned long long);
                kprint_u64_hex((uint64_t)v, width, zero_pad);
            } else if (len_l == 1) {
                unsigned long v = va_arg(vargs, unsigned long);
                kprint_u64_hex((uint64_t)v, width, zero_pad);
            } else {
                unsigned v = va_arg(vargs, unsigned);
                /* 32bit値でも64ビット出力関数に任せる */
                kprint_u64_hex((uint64_t)v, width, zero_pad);
            }
            break;
        }

        case 'p': { /* pointer: 0x + 16桁表示想定 */
            uint64_t v = (uint64_t)va_arg(vargs, void *);
            console_putc('0'); console_putc('x');
            kprint_u64_hex((uint64_t)v, (width ? width : 16), 1);
            break;
        }

        default:
            /* 未サポート指定子はそのまま可視化 */
            console_putc('%');
            if (len_l == 1) console_putc('l');
            else if (len_l == 2) { console_putc('l'); console_putc('l'); }
            if (zero_pad) console_putc('0');
            if (width) {
                /* 幅数字を簡易に出す（厳密に復元しない） */
                if (width >= 10) console_putc('0' + (width / 10) % 10);
                console_putc('0' + (width % 10));
            }
            console_putc(sp ? sp : '?');
            break;
        }
    }

end:
    va_end(vargs);
}