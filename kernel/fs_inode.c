#include "fs.h"
#include "printk.h"
#include "string.h"

/* グローバル変数 */
extern struct superblock g_sb;
static struct inode g_icache[NINODE];

/* ユーティリティおよびヘルパー関数 */
#define IPB (BSIZE / sizeof(struct dinode)) // inode 一個あたりのサイズ
#define ROOT_INO 1

//inode番号からディスク上dinodeへのマッピングを行う
static inline uint32_t inode_blockno(uint32_t inum)
{
    // inode 番号 inum は 1-origin とする (inum=1 が最初の inode)
    return g_sb.inodestart + (inum / IPB);
}

static inline uint32_t inode_block_index(uint32_t inum)
{
    return inum % IPB;
}

//!
//! inodeの論理ブロック番号bnに対応するディスク上の物理ブロック番号を返す
//! 現在は読み取り専用で実装を行うので、新規の割り当ては行わない。
//!
static uint32_t bmap(struct inode *ip, uint32_t bn)
{
    if (bn < NDIRECT)
    {
        return ip->addrs[bn];   // 0の場合割り当てなしとなる
    }

    // 間接ブロックは後で
    printk("bmap: indirect not implemented\n");
    return 0;
}

static int namecmp(const char *s, const char *t)
{
    return strncmp(s, t, DIRSIZ);
}

//!
//! @brief パスを分解するヘルパー関数
//! @brief pathの先頭から1要素取り出してnameに入れ、pathの残りのポインタを返す。
//! @param path 分解するパス
//! @param name 取り出したパス名
//! @return nameを取り出した後の残りのpath
//!
static const char *skipelem(const char *path, char *name)
{
    const char *s;
    int len;

    // 先頭の'/'を飛ばす
    while(*path == '/')
    {
        path++;
    }
    if (*path == 0)
    {
        return NULL;
    }

    s = path;

    while(*path != '/' && *path != 0)
    {
        path++;
    }
    len = path - s;
    if (len >= DIRSIZ)
    {
        len = DIRSIZ - 1;
    }
    memcpy(name, s, len);
    name[len] = 0;

    // 次の要素のために'/'をスキップ
    while (*path == '/')
    {
        path++;
    }
    return path;
}

/* APIの実装 */
void inode_init(void)
{
    // icacheの初期化
    memset(&g_icache, 0, sizeof(g_icache));
}

struct inode *iget(uint32_t inum)
{
    struct inode *empty = NULL;

    for (int i = 0; i < NINODE; i++)
    {
        // キャッシュからinodeを取得する
        struct inode *ip = &g_icache[i];

        if (ip->ref > 0 && ip->inum == inum)
        {
            // キャッシュヒット
            ip->ref++;
            return ip;
        }

        if (empty == NULL && ip->ref == 0)
        {
            // キャッシュミス
            empty = ip;
        }
    }

    if (empty == NULL)
    {
        // 該当するinodeが存在しないのでエラー
        printk("iget: no inodes");
        return (struct inode*)NULL;
    }

    // キャッシュに現在のinodeを新規登録
    empty->dev = 0;
    empty->inum = inum;
    empty->ref = 1;
    empty->valid = 0;

    return empty;
}

void ilock(struct inode *ip)
{
    if (ip == NULL)
        return;
    if (ip->valid)
        return;

    uint8_t buf[BSIZE];

    uint32_t blockno = inode_blockno(ip->inum);
    uint32_t index = inode_block_index(ip->inum);

    if (bdev_read(blockno, buf) != 0)
    {
        printk("ilock: failed to read inode block!\n");
        return;
    }

    struct dinode *dip = (struct dinode*)buf;
    dip += index;

    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->size  = dip->size;
    memcpy(ip->addrs, dip->addrs, sizeof(ip->addrs));

    ip->valid = 1;
}

void iunlock(struct inode *ip)
{
    // 現状は何もしない
}

int readi(struct inode *ip, void *dst, uint32_t off, uint32_t n)
{
    if (ip->type == 0)
        return -1;

    // オフセットがファイルサイズを越えないようにする
    if (off > ip->size)
        return 0;
    if (off + n > ip->size)
        n = ip->size - off;
    
    uint8_t *dst8 = (uint8_t *)dst;
    uint32_t tot = 0;

    while (tot < n)
    {
        uint32_t bn = (off + tot) / BSIZE;      // 論理ブロック番号
        uint32_t boff = (off + tot) % BSIZE;    // ブロック内オフセット

        uint32_t bno = bmap(ip, bn);
        if (bno == 0) {
            break;
        }

        uint8_t buf[BSIZE];
        if (bdev_read(bno, buf) != 0) {
            printk("readi: bdev_read failed\n");
            break;
        }

        uint32_t m = BSIZE - boff;
        if (m > n - tot) {
            m = n - tot;
        }

        memcpy(dst8 + tot, buf + boff, m);
        tot += m;
    }

    return tot;
}

static void fs_test_write_fake_inode_and_data(void)
{
    // superblockと同様にテスト用inodeを作成する
    const char *msg = "Hello from inode!\n";
    uint32_t msg_len = (uint32_t)strlen(msg);

    // データブロック番号を適当に決める（superblock で予約と被らないあたり）
    uint32_t data_block = g_sb.bmapstart + 1; // 適当: 今はビットマップ使ってないのでOK

    // 1. データブロックにメッセージを書き込む
    uint8_t buf[BSIZE];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, msg, msg_len);
    bdev_write(data_block, buf);

    // 2. inode ブロックを読み込み
    uint8_t ibuf[BSIZE];
    uint32_t inum = 1;          // テスト用 inode 番号
    uint32_t ibno = inode_blockno(inum);
    uint32_t idx  = inode_block_index(inum);

    if (bdev_read(ibno, ibuf) != 0) {
        printk("fs: failed to read inode block for test\n");
        return;
    }

    struct dinode *dip = (struct dinode *)ibuf;
    dip += idx;

    memset(dip, 0, sizeof(*dip));
    dip->type  = T_FILE;
    dip->nlink = 1;
    dip->size  = msg_len;
    dip->addrs[0] = data_block;  // 直接ブロックの先頭に data_block をセット

    if (bdev_write(ibno, ibuf) != 0) {
        printk("fs: failed to write inode block for test\n");
        return;
    }
}

void fs_test_inode(void)
{
    printk("fs: inode test start\n");

    // テスト用 inode + data を書き込む
    fs_test_write_fake_inode_and_data();

    // inode キャッシュ初期化
    inode_init();

    // inode 1 を取ってくる
    struct inode *ip = iget(1);
    if (!ip) {
        printk("fs: inode test failed: iget\n");
        return;
    }

    ilock(ip);

    char buf[64];
    int n = readi(ip, buf, 0, sizeof(buf) - 1);
    if (n < 0) {
        printk("fs: inode test failed: readi\n");
        return;
    }
    buf[n] = '\0';

    printk("fs: inode content: \"%s\"\n", buf);

    if (strcmp(buf, "Hello from inode!\n") != 0) {
        printk("fs: inode test failed: content mismatch\n");
        return;
    }

    printk("fs: inode test [OK]\n");
}

struct inode *dirlookup(struct inode *dp, const char *name, uint32_t *poff)
{
    if (dp->type != T_DIR)
    {
        printk("dirlookup: this is not a dir");
        return (struct inode*)NULL;
    }

    struct dirent de;
    uint32_t off;

    for (off = 0; off < dp->size; off += sizeof(struct dirent))
    {
        int n = readi(dp, &de, off, sizeof(de));
        if (n != sizeof(de))
        {
            printk("dirlookup: readi error\n");
            return (struct inode*)NULL;
        }
        if (de.inum == 0)
        {
            // 空のエントリー
            continue;
        }
        if (namecmp(de.name, name) == 0) 
        {
            // 見つかった
            if (poff) 
            {
                *poff = off;
            }
            return iget(de.inum);
        }
    }
    return (struct inode*)NULL;
}

struct inode *namei(const char *path)
{
    struct inode *ip;
    char name[DIRSIZ];
    const char *p;

    if (*path == '/')
    {
        ip = iget(ROOT_INO);    // ルートから開始
    }
    else
    {
        // 現状の実装ではルート固定にしてある
        ip = iget(ROOT_INO);
    }

    while((p = skipelem(path, name)) != NULL)
    {
        path = p;
        ilock(ip);
        if (ip->type != T_DIR)
        {
            printk("namei: not dir in path\n");
            return NULL;
        }
        struct inode *next = dirlookup(ip, name, NULL);

        iunlock(ip);
        if (next == NULL)
        {
            return NULL;
        }
        ip = next;
    }
    return ip;
}

#define ROOT_INO  1
#define HELLO_INO 2

static void fs_test_write_fake_root_and_file(void)
{
    const char *msg = "Hello from inode!\n";
    uint32_t msg_len = (uint32_t)strlen(msg);

    // 適当なブロック番号を選ぶ
    uint32_t data_block = g_sb.bmapstart + 1;   // /hello の中身
    uint32_t dir_block  = g_sb.bmapstart + 2;   // ルートディレクトリのエントリ

    uint8_t buf[BSIZE];

    // 1. /hello のデータブロックを書き込む
    memset(buf, 0, sizeof(buf));
    memcpy(buf, msg, msg_len);
    bdev_write(data_block, buf);

    // 2. ルートディレクトリのデータブロック（dirent）を書く
    struct dirent de[3];

    // "."
    de[0].inum = ROOT_INO;
    memset(de[0].name, 0, DIRSIZ);
    strncpy(de[0].name, ".", DIRSIZ);

    // ".."（ルートなので親も自分）
    de[1].inum = ROOT_INO;
    memset(de[1].name, 0, DIRSIZ);
    strncpy(de[1].name, "..", DIRSIZ);

    // "hello"
    de[2].inum = HELLO_INO;
    memset(de[2].name, 0, DIRSIZ);
    strncpy(de[2].name, "hello", DIRSIZ);

    memset(buf, 0, sizeof(buf));
    memcpy(buf, de, sizeof(de));
    bdev_write(dir_block, buf);

    // 3. inode ブロックを読み出して、ROOT_INO と HELLO_INO を書き込む
    uint8_t ibuf[BSIZE];

    // a) ROOT_INO
    uint32_t bno = inode_blockno(ROOT_INO);
    uint32_t idx = inode_block_index(ROOT_INO);

    if (bdev_read(bno, ibuf) != 0) 
    {
        printk("fs: failed to read inode block (root)\n");
        return;
    }

    struct dinode *dip = (struct dinode *)ibuf;
    dip += idx;

    memset(dip, 0, sizeof(*dip));
    dip->type  = T_DIR;
    dip->nlink = 1;
    dip->size  = sizeof(de);    // 3エントリ分
    dip->addrs[0] = dir_block;

    if (bdev_write(bno, ibuf) != 0) 
    {
        printk("fs: failed to write inode block (root)\n");
        return;
    }

    // b) HELLO_INO
    bno = inode_blockno(HELLO_INO);
    idx = inode_block_index(HELLO_INO);

    if (bdev_read(bno, ibuf) != 0) 
    {
        printk("fs: failed to read inode block (hello)\n");
        return;
    }

    dip = (struct dinode *)ibuf;
    dip += idx;

    memset(dip, 0, sizeof(*dip));
    dip->type  = T_FILE;
    dip->nlink = 1;
    dip->size  = msg_len;
    dip->addrs[0] = data_block;

    if (bdev_write(bno, ibuf) != 0) 
    {
        printk("fs: failed to write inode block (hello)\n");
        return;
    }
}

void fs_test_namei(void)
{
    printk("fs: namei test start\n");

    // テスト用にディスク内容を構築
    fs_test_write_fake_root_and_file();

    // inode キャッシュ初期化
    inode_init();

    struct inode *ip = namei("/hello");
    if (ip == NULL) 
    {
        printk("fs: namei test failed: namei returned NULL\n");
        return;
    }

    ilock(ip);

    char buf[64];
    int n = readi(ip, buf, 0, sizeof(buf) - 1);
    if (n < 0) 
    {
        printk("fs: namei test failed: readi error\n");
        return;
    }
    buf[n] = '\0';

    printk("fs: namei /hello content: \"%s\"\n", buf);

    if (strcmp(buf, "Hello from inode!\n") != 0) 
    {
        printk("fs: namei test failed: content mismatch\n");
        return;
    }

    printk("fs: namei test [OK]\n");
}

