#ifndef FS_H
#define FS_H

#include <stdint.h>

#define FS_NAME_MAX     32
#define FS_DIRECT_MAX    8

typedef uint32_t fs_ino_t;

typedef enum {
    FS_T_NONE = 0,
    FS_T_REG  = 1,
    FS_T_DIR  = 2
} fs_type_t;

/*
 *
 */
typedef struct {
    uint32_t magic;
    uint32_t block_size;
    uint32_t block_count;
    fs_ino_t root_ino;
    uint32_t version;
    uint32_t features;
} fs_super_t;


typedef struct {
    char     name[FS_NAME_MAX];
    fs_ino_t ino;
    uint8_t  type;
} fs_dirent_t;

/*
 *
 */
typedef struct {
    uint8_t  type;
    uint32_t size;
    uint32_t nblocks;
    uint32_t blocks[FS_DIRECT_MAX];
    uint32_t csum;
} fs_inode_disk_t;

typedef struct fs_inode {
    fs_ino_t ino;
    uint8_t  type;
    uint32_t size;
    uint32_t nblocks;
    uint32_t blocks[FS_DIRECT_MAX];
    struct fs_super *sb;
} fs_inode_t;


typedef struct {
    void *ctx;
    int (*read)(void *ctx, uint32_t blk, uint32_t off, void *buf, uint32_t len);
    int (*prog)(void *ctx, uint32_t blk, uint32_t off, const void *buf, uint32_t len);
    int (*erase)(void *ctx, uint32_t blk);
    int (*sync)(void *ctx);
} fs_blkdev_t;

int fs_mount(fs_super_t *sb, fs_blkdev_t *bdev);
int fs_unmount(fs_super_t *sb);
int fs_format(fs_super_t *sb);
int fs_sync(fs_super_t *sb);

/*
 *
 */
int fs_resolve(fs_super_t *sb, const char *path, fs_inode_t **out);

int fs_mkdir(fs_super_t *sb, const char *path);
int fs_rmdir(fs_super_t *sb, const char *path);
int fs_readdir(fs_super_t *sb, const char *path, fs_dirent_t *buf, uint32_t max, uint32_t *nread);

int fs_create(fs_super_t *sb, const char *path);
int fs_unlink(fs_super_t *sb, const char *path);
int fs_open(fs_super_t *sb, const char *path, fs_inode_t **out);
int fs_read(fs_inode_t *inode, uint32_t off, void *buf, uint32_t len);
int fs_write(fs_inode_t *inode, uint32_t off, const void *buf, uint32_t len);
int fs_truncate(fs_inode_t *inode, uint32_t newsize);

int inode_alloc(fs_super_t *sb, fs_inode_t **out);
int inode_free(fs_super_t *sb, fs_inode_t *inode);
int inode_read(fs_super_t *sb, uint32_t ino, fs_inode_t *out);
int inode_write(fs_super_t *sb, fs_inode_t *in);

int dir_add_entry(fs_inode_t *dir, const char *name, uint32_t ino, uint8_t type);
int dir_remove_entry(fs_inode_t *dir, const char *name);
int dir_lookup(fs_inode_t *dir, const char *name, fs_dirent_t *out);

#endif 
