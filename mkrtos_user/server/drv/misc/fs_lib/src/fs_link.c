#include <fs.h>

struct inode_operations sp_symlink_inode_operations = {
    NULL,           /* no file-operations */
    NULL,           /* create */
    NULL,           /* lookup */
    NULL,           /* link */
    NULL,           /* unlink */
    NULL,           /* symlink */
    NULL,           /* mkdir */
    NULL,           /* rmdir */
    NULL,           /* mknod */
    NULL,           /* rename */
    sp_readlink,    /* readlink */
    sp_follow_link, /* follow_link */
    NULL,           /* bmap */
    NULL,           /* truncate */
    NULL            /* permission */
};
