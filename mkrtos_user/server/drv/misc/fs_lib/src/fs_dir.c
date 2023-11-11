#include <fs.h>

static int fs_readdir(struct inode *inode, struct file *filp,
               struct dirent *dirent, int count)
{
   return -1;
}
static int file_fsync (struct inode *inode, struct file *filp)
{
   return 0;
}
static struct file_operations sp_dir_operations = {
    .lseek = NULL,         /* lseek - default */
    .read = NULL,          /* read */
    .write = NULL,         /* write - bad */
    .readdir = fs_readdir, /* readdir */
    .select = NULL,        /* select - default */
    .ioctl = NULL,         /* ioctl - default */
    .mmap = NULL,          /* mmap */
    .mumap = NULL,
    .open = NULL,       /* no special open code */
    .release = NULL,    /* no special release code */
    .fsync = file_fsync /* default fsync */
};

/*
 * directories can handle most operations...
 */
struct inode_operations sp_dir_inode_operations = {
    &sp_dir_operations,      /* default directory file-ops */
    .create = sp_create,     /* create */
    .lookup = sp_lookup,     /* lookup */
    .link = sp_link,         /* link */
    .unlink = sp_unlink,     /* unlink */
    .symlink = sp_symlink,   /* symlink */
    .mkdir = sp_mkdir,       /* mkdir */
    .rmdir = sp_rmdir,       /* rmdir */
    .mknod = sp_mknod,       /* mknod */
    .rename = sp_rename,     /* rename */
    .readlink = NULL,        /* readlink */
    .follow_link = NULL,     /* follow_link */
    .bmap = NULL,            /* bmap */
    .truncate = sp_truncate, /* truncate */
    .permission = NULL       /* permission */
};
