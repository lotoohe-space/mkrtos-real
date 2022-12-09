

#ifndef _CPIO_FS_H__
#define _CPIO_FS_H__

#include <type.h>
#include <mkrtos/fs.h>

//! 26bytes
#pragma pack (1)
typedef struct cpio_fs {
    char    c_magic[6];
    char    c_ino[8];
    char    c_mode[8];
    char    c_uid[8];
    char    c_gid[8];
    char    c_nlink[8];
    char    c_mtime[8];
    char    c_filesize[8];
    char    c_devmajor[8];
    char    c_devminor[8];
    char    c_rdevmajor[8];
    char    c_rdevminor[8];
    char    c_namesize[8];
    char    c_check[8];
} cpio_fs_t; 

typedef struct cpio_fs_ino_priv {
    cpio_fs_t fs_info;
    uint32_t st_bk; //!<
    uint32_t in_bk_offs;
} cpio_fs_ino_priv_t;

typedef struct cpio_sb {
    // uint32_t bk_cn;//!< bk数量
} cpio_sb_t;

extern struct inode_operations cpio_file_inode_operations;
extern struct inode_operations cpio_dir_inode_operations;
int htoi(char* str, int len);

struct super_block* cpio_read_sb(struct super_block *sb);
int32_t cpio_check_inode_no(struct super_block *sb, ino_t ino, cpio_fs_ino_priv_t *cpio_info_fs);
int32_t cpio_find_file(struct super_block *sb, const char *name, cpio_fs_ino_priv_t *cpio_info_fs);
int32_t cpio_root_file_count(struct super_block *sb, cpio_fs_ino_priv_t *cpio_info_fs, uint32_t *count);

#endif
