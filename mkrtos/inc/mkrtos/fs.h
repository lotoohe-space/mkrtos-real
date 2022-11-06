//
// Created by Administrator on 2022/1/10.
//

#ifndef UNTITLED1_FS_H
#define UNTITLED1_FS_H

#include <errno.h>
#include <type.h>
#include <arch/atomic.h>

//��ǰ���̸�Ŀ¼INODE
#define ROOT_INODE ((struct inode*)(sysTasks.currentTask->root_inode))
//��ǰ���̹���Ŀ¼INODE
#define PWD_INODE ((struct inode*)(sysTasks.currentTask->pwd_inode))

struct file_operations;
struct inode_operations;
struct super_block;

#define MK_MODE(a,b) (((a))|(b))
//ȡ���ļ�����
#define FILE_TYPE(a) (((a)>>16)&0xffff)
//ȡ���ļ�ģʽ
#define FILE_MODE(a) ((a)&0xffff)

typedef struct {
    long    val[2];
} fsid_t;

/* �ļ�ϵͳ����Ϣ�ṹ */
struct statfs {
	long f_type;
	long f_bsize;
	long f_blocks;
	long f_bfree;
	long f_bavail;
	long f_files;
	long f_ffree;
	fsid_t f_fsid;
	long f_namelen;
	long f_frsize;
	long f_spare[5];
};

struct wait_queue;
//INode�ڵ�
typedef struct inode {
    //�ļ�������Ȩ��
    //
    uint32_t i_type_mode;
    //�ļ���С
    uint32_t i_file_size;
    //�Լ���Inode����
    uint32_t i_no;
    //Ӳ������
    uint32_t i_hlink;
    //������豸�ļ�������������豸��
    uint32_t i_rdev_no;

    //�����������Ҫ�浽����
    struct inode *i_mount;
    struct wait_queue *i_wait_q;//���inode�ĵȴ�����
    Atomic_t i_lock;//���������inode
    //�򿪼���
//    Atomic_t i_open_count;
    Atomic_t i_used_count;//ʹ�ü���
    struct wait_queue *poll_wait;//poll_wait��inode �Ķ���
//    struct wait_queue *w_wait;//�ȴ�д���inode �Ķ���
    uint8_t i_dirt;//�Ƿ��޸Ĺ�
    struct super_block *i_sb; //inode��sb
    struct inode_operations* i_ops;//inode��ops
    //�ļ�ϵͳ��ţ�ͨ���ñ�Ų��ҵ���Ӧ���ļ�ϵͳ
//    uint32_t i_fs_no;
    void *i_fs_priv_info;//�ļ�ϵͳӵ�е�˽����Ϣ
}*p_inode, inode;

//SupberBlock�ڵ�
typedef struct super_block {
    //ħ��
    uint32_t s_magic_num;
    //���С
    uint32_t s_bk_size;
    //���豸���豸��
    uint32_t s_dev_no;
    //inodeд�뵽���̵Ĵ�С
    uint16_t s_inode_size;

    //�ļ�ϵͳ��ţ�ͨ���ñ�Ų��ҵ���Ӧ���ļ�ϵͳ
    uint32_t s_fs_no;
    //�ļ�ϵͳ���ڵ��inode
    struct inode *root_inode;
    struct inode *s_covered;
    //�Ƿ��޸Ĺ�
    uint8_t s_dirt;
    //super��������
    struct super_operations* s_ops;
    //�ļ�ϵͳӵ�е�˽����Ϣ
    void *s_sb_priv_info;
}*p_super_block, super_block;


struct file {
    uint32_t f_mode;		    /* �ļ�������ʱ�������ļ���Ȩ�� */
    int32_t f_ofs;            /* �ļ���дƫ���� */
    dev_t  f_rdev;             /* ����tty�豸�����ڵ��ַ��豸������ */
//    uint32_t f_count;           /*���file��ʹ���˶��ٴ�,��ʱ�ò���*/
    unsigned short f_flags; /* ��ʲô���ķ�ʽ���ļ�����ֻ����ֻд�ȵ� */
    struct inode * f_inode;		/* �ļ���Ӧ��inode */
    struct file_operations * f_op; /*�ļ���Ӧ�Ĳ�����*/
    uint32_t used;   /*�Ƿ�ʹ�ñ��*/

    int fo_flag;

    int net_sock;/*����sock*/
    uint8_t net_file;/*�����ļ�*/
//    uint32_t sock_used_cn;/*�����ļ�ʹ�ü���*/
};

#include <dirent.h>
//struct dirent
//{
//    long d_ino; /*�����ڵ�� */
//    int32_t d_off; /*��Ŀ¼�ļ��е�ƫ�� */
//    uint16_t d_file_name_len; /*�ļ����� */
////    unsigned char d_type; /*�ļ����� */
//    char d_name [128+1]; /*�ļ������128�ַ� */
//};

/* ��inode��Ӧ�ļ��Ĳ���
 */
struct file_operations {
    int (*lseek) (struct inode *, struct file *, off_t, int);
    int (*read) (struct inode *, struct file *, char *, int);
    int (*write) (struct inode *, struct file *, char *, int);
    int (*readdir) (struct inode *, struct file *, struct dirent *, int);
//    int (*select) (struct inode *, struct file *, int, uint32_t *);
    int (*poll) (struct inode * inode,struct file *fp, void *wait_tb,struct timeval *timeout);
    int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
    int (*mmap) (struct inode * inode, struct file * fp, unsigned long addr, size_t len , int mask, unsigned long off);
    int (*mumap)(struct inode * inode,void * start ,size_t len);
    int (*open) (struct inode * inode, struct file * fp);
    void (*release) (struct inode * ino, struct file * f);
    int (*fsync) (struct inode *, struct file *);
};

/* ��inode�Ĳ���
 */
struct inode_operations {
    struct file_operations * default_file_ops;
    int (*create) (struct inode *dir,const char *name,int len,int mode,struct inode **result);
    int (*lookup) (struct inode * dir,const char * file_name,int len,struct inode ** ret_inode);
    int (*link) (struct inode *,struct inode *,const char *,int);
    int (*unlink) (struct inode *,const char *,int);
    int (*symlink) (struct inode *,const char *,int,const char *);
    int (*mkdir) (struct inode *,const char *,int,int);
    int (*rmdir) (struct inode *,const char *,int);
    int (*mknod) (struct inode *,const char *,int,int,int);
    int (*rename) (struct inode *,const char *,int,struct inode *,const char *,int);
    int (*readlink) (struct inode *,char *,int);
    int (*follow_link) (struct inode *,struct inode *,int,int,struct inode **);
    int (*bmap) (struct inode *,int);
    void (*truncate) (struct inode *,int);
    int (*permission) (struct inode *, int);
};

struct super_operations {
    //����ָ���ļ�ϵͳ��inode
    struct inode * (*alloc_inode)(struct inode * p_inode);
    //�ͷ�ָ���ļ�ϵͳ��inode
    int32_t (*free_inode)(struct inode *p_inode);

    int32_t (*read_inode) (struct inode * p_r_ino);
    int (*notify_change) (int flags, struct inode *);
    void (*write_inode) (struct inode *);
    void (*put_inode) (struct inode *);
    void (*put_super) (struct super_block *);
    void (*write_super) (struct super_block *);
    void (*statfs) (struct super_block *, struct statfs *);
    int (*remount_fs) (struct super_block *, int *, char *);
};



struct fs_type{
    //�ļ�ϵͳ������
    const char* f_name;
    //�Ƿ���Ҫ�豸��
    uint8_t req_dev_no;
    //������ļ�ϵͳ�Ĳ�����
    struct super_block * (*read_sb)(struct super_block * sb);
    struct super_block* (*alloc_sb)(struct super_block* sb);
    void (*free_sb)(struct super_block* sb);
};

//fs_type.c�ļ�ϵͳ�������
extern struct fs_type fs_type_list[];
extern int32_t fs_type_len;
struct fs_type *find_fs_type(const char *fs_name);

//open.c
//
//void sys_close(int fd);

//inode.c ��Ŀ¼���豸��
void lose_inode(struct inode* p_inode);
struct inode* get_empty_inode(void);
struct inode* geti(struct super_block* p_sb,ino_t ino);
int32_t puti(struct inode* put_inode);

void wait_on_inode(struct inode* inode);
void lock_inode(struct inode* inode);
void unlock_inode(struct inode* inode);
int sync_all_inode(void);


//namei.c
int namei(const char * pathname, struct inode ** res_inode);
int lnamei(const char * pathname, struct inode ** res_inode);
int32_t dir_namei(const char * pathname, int32_t * namelen, const char ** name,
                  struct inode * base, struct inode ** res_inode);
int32_t open_namei(const char* file_path,int32_t flags,int32_t mode,struct inode **res_inode,struct inode *base_dir);


//dev.c
extern dev_t root_dev_no;
extern struct inode_operations chrdev_inode_operations;
extern struct inode_operations blkdev_inode_operations;
struct bk_operations{
    int32_t (*open)(uint32_t bk_no);
    int32_t (*read_bk)(uint32_t bk_no,uint8_t *data);
    int32_t (*write_bk)(uint32_t bk_no,uint8_t *data);
    int32_t (*erase_bk)(uint32_t bk_no);
    void (*release)(uint32_t bk_no);
};
int32_t get_bk_cn(dev_t major_no,uint32_t *res_bk_no);
uint32_t get_bk_size(dev_t major_no) ;
struct bk_operations* get_bk_ops(dev_t major_no);
uint32_t get_bk_count(dev_t major_no);
struct bk_cache* get_bk_dev_cache(dev_t major_no,uint32_t *res_cache_len);
int32_t reg_bk_dev(
        dev_t major_no,
        const char* name ,
        struct bk_operations *bk_ops,
        struct file_operations *file_ops
//                ,
//        uint32_t bk_count,
//        uint32_t cache_len,
//        uint32_t bk_size
);
int32_t bk_dev_reg_param(dev_t major_no,uint32_t bk_count,uint32_t cache_len,uint32_t bk_size) ;
int32_t bk_dev_unreg_param(dev_t major_no) ;
int32_t unreg_bk_dev(dev_t major_no,const char* name);
struct file_operations* get_ch_dev_ops(dev_t major_no);
int32_t reg_ch_dev(dev_t major_no,const char* name ,struct file_operations *d_fops);
int32_t unreg_ch_dev(dev_t major_no,const char* name);
int32_t request_bk_no(dev_t dev_no);
int32_t alloc_bk_no();
int32_t request_char_no(dev_t dev_no);
void lock_bk_ls(dev_t bk_dev_no);
void unlock_bk_ls(dev_t bk_dev_no);
void devs_init(void);
int sync_all_bk_dev(void) ;
/////////

//bk.c
struct wait_queue;

//�黺�����
struct bk_cache {
    //���Inx
    uint32_t bk_no;
    uint32_t bk_size;
    //�����������Ҫ���棬��ΪNull
    uint8_t* cache;
    uint8_t* oldcache;
    Atomic_t b_lock;
    //ʹ�ü���
    uint32_t used_cn;
    //�ȴ�����
    struct wait_queue* b_wait;
    //������� 1bitд�� 2bit��ȡ (4bit ������ס�����Ǳ�ʹ�����ͷŵ�) (5bit mmapӳ��)  (6bit 7bit 00Ϊ�� 01��ʹ�� 10��ɾ��)
    uint8_t flag;
};
#define KEY_ACCESS(a) ((a)&0xC0)
//�Ƿ�֧��ĳ�ַ�������
#define IS_NULLKEY(a)   (KEY_ACCESS((a).flag)==0x00)
#define IS_USEDKEY(a)   (KEY_ACCESS((a).flag)==0x40)
#define IS_DELKEY(a)    (KEY_ACCESS((a).flag)==0x80)

//�Ƿ�mmap��
#define IS_MMAP(a)      ((a).flag&((1<<5)))
//�Ƿ񶳽���
#define IS_FREEZE(a)    ((a).flag&((1<<4)))

#define SET_FREEZE(a)   (a).flag&=(~(1<<4));(a).flag|=(1<<4)
//mmapλ���ú�freezeһ��Ҫ����
#define SET_MMAP(a)     SET_FREEZE(a);(a).flag&=(~(1<<5));(a).flag|=(1<<5)

#define SET_NULLKEY(a)  (a).flag&=~0xC0
#define SET_USEDKEY(a)  (a).flag&=~0xC0;(a).flag|=0x40
#define SET_DELKEY(a)   (a).flag&=~0xC0;(a).flag|=0x80
#define CLR_KEY(a)  (a).flag=0;
////////


//super.c
struct super_block* get_empty_sb(void);
void free_sb(struct super_block* sb);
int sync_all_sb(void);

//super.c
struct task;
void root_mount(struct task* task);

//file_name.c
int32_t _getname(const char* file_name,uint32_t len,char** to);
int32_t getname(const char* file_name, char** to);
void putname(const char* file_name);
char* path_name(char* file_path);

//mmap.c
int general_mmap(struct inode * inode, struct file * fp, void* addr, size_t len , int mask, unsigned long off);
int general_mumap(struct inode * inode,void * start ,size_t len);

//bk_rw.c
int bk_file_open(struct inode * ino, struct file * filp);
int bk_file_read(struct inode *ino, struct file *fp, char * buf, int count);
int bk_file_write(struct inode *ino, struct file * fp, char * buf, int count);
int bK_file_fsync(struct inode *inode, struct file *filp);
int bk_file_release(struct inode * ino, struct file * fp);

/**
 * @breif: ��ͬ������Setion�Ķ���
 */
#ifndef SECTION
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
#define SECTION(x)                  __attribute__((section(x)))
#elif defined(__ICCARM__) || defined(__ICCRX__)
#define SECTION(x)                  @ x
#elif defined(__GNUC__)
#define SECTION(x)                  __attribute__((section(x)))
#else
#define SECTION(x)
#endif
#endif

struct dev_reg{
    int32_t (*init)(void);
    int32_t (*exit)(void);
};
/**
 * @breif dev���ٳ�ʼ��
 */
#define DEV_BK_EXPORT(_initFun,_exitFun,_name) \
        const struct dev_reg \
        dev_reg_##_name SECTION("DevRegList") = {\
			.init=_initFun,\
			.exit=_exitFun\
		}

void trace(const char* fmt, ...);
void printk(const char *fmt, ...);
void fatalk(const char* fmt, ...);
void console_write(const char* str);

#endif //UNTITLED1_FS_H
