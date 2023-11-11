
#include <fs.h>

static int fs_ioctl(struct inode *ino, struct file *fd, unsigned int cmd, unsigned long arg)
{

    return 0;
}

int fs_file_read(struct inode *inode, struct file *filp, char *buf, int count)
{

    return -1;
}
int fs_file_write(struct inode *inode, struct file *filp, char *buf, int count)
{
    return 0;
}
int fs_sync_file(struct inode *inode, struct file *file)
{

    return 0;
}
static struct file_operations fs_file_operations = {
    .lseek = NULL,          /* lseek - default */
    .read = fs_file_read,   /* read */
    .write = fs_file_write, /* write */
    .readdir = NULL,        /* readdir */
    .select = NULL,         /* select - default */
    .ioctl = fs_ioctl,      /* ioctl */
    .mmap = NULL,           /* mmap */
    .mumap = NULL,          /*mumap*/
    .open = NULL,           /* no special open code */
    .release = NULL,        /* no special release code */
    .fsync = fs_sync_file   /* default fsync */
};

/*
 * directories can handle most operations...
 */
struct inode_operations fs_file_inode_operations = {
    &fs_file_operations, /* default directory file-ops */
    .create = NULL,      /* create */
    .lookup = NULL,      /* lookup */
    .link = NULL,        /* link */
    .unlink = NULL,      /* unlink */
    .symlink = NULL,     /* symlink */
    .mkdir = NULL,       /* mkdir */
    .rmdir = NULL,       /* rmdir */
    .mknod = NULL,       /* mknod */
    .rename = NULL,      /* rename */
    .readlink = NULL,    /* readlink */
    .follow_link = NULL, /* follow_link */
    .bmap = NULL,        /* bmap */
    .truncate = NULL,    /* truncate */
    .permission = NULL   /* permission */
};
