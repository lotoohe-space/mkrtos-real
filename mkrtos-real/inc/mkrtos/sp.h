//
// Created by Administrator on 2022/1/12.
//

#ifndef UNTITLED1_SP_H
#define UNTITLED1_SP_H

#include <type.h>
#include <mkrtos.h>
#define SP_MAGIC_NUM 0x11223344

//һ�����п��Դ�Ŷ��ٸ�INode
#define INODE_NUM_IN_BK(a) ((a)->s_bk_size/(a)->s_inode_size)

//sp�ļ�ϵͳ��˽����Ϣ
#define SP_INODE(a) ((struct sp_inode*)((a)->i_fs_priv_info))
//һ��BK�����԰������ٸ�INode����BK��
#define BK_INC_X_NUM(sb) ((sb)->s_bk_size / sizeof(bk_no_t))
//һ���������ж����ļ���
#define DIR_NUM(p_inode) (p_inode->i_sb->s_bk_size/sizeof(struct dir_item))
//�ļ�ʹ���˶��ٿ�
#define INODE_BK_NUM(p_inode) ROUND_UP(p_inode->i_file_size, p_inode->i_sb->s_bk_size)
//һ��bk�����
#define A_BK_NUM(p_inode)	ARRARY_LEN((p_inode)->p_ino)
//����bk�����
#define B_BK_NUM(sb,p_inode)	(BK_INC_X_NUM(sb) * ARRARY_LEN((p_inode)->pp_ino))
//����bk�����
#define C_BK_NUM(sb,p_inode)  BK_INC_X_NUM(sb) * BK_INC_X_NUM(sb) * ARRARY_LEN(p_inode->ppp_ino)
//�ļ�ʹ���˶��ٿ�
#define FILE_USED_BK_NUM(sb,p_inode) ROUND_UP(p_inode->i_file_size, sb->s_bk_size)
//һ���������ж����ļ���
#define DIR_ITEM_NUM_IN_BK(a) ((a)->s_bk_size/sizeof(struct dir_item))

#define DIR_ITEM_SIZE 128
//�洢��Ŀ¼�������2��������
struct dir_item {
	char name[DIR_ITEM_SIZE - sizeof(di_size_t) - sizeof(uint32_t)];
	di_size_t inode_no;
	uint32_t used;
};

//������sp�ļ�ϵͳ��˽��inode��Ϣ
struct sp_inode {
	union {
		ino_t p_ino[12];
		struct strit { //!<�ϸ�ģʽʹ��
			bk_no_t start_bk; //!<��ʼ��bkλ��
			uint32_t bk_num; //!<bk������
		}_l;
	};

	ino_t pp_ino[6]; //!<������ppָ��
	ino_t ppp_ino[3]; //!<������pppָ��
};

//sp�ļ�ϵͳ˽�е�sb��Ϣ
struct sp_super_block {
	//������
	uint32_t block_count;
	//�������
	uint32_t block_free;

	//iNode��С
	uint32_t inode_size;
	//iNode����
	uint32_t inode_count;
	//iNode����
	uint32_t inode_free;

	//�ļ�ϵͳ��Ϣ������λ��
	uint32_t fs_info_st_bk_inx;

	//iNodeʹ�ÿ�����λ��
	uint32_t inode_used_bk_st_inx;
	//iNodeʹ�ÿ� ռ�ö��ٿ�
	uint32_t inode_used_bk_count;

	//bkʹ�ÿ�����λ��
	uint32_t bk_used_bk_st_inx;
	//bkʹ�ÿ� ռ���˶��ٿ�
	uint32_t bk_used_bk_count;

	//iNode���ݿ�ʼλ��
	uint32_t inode_data_bk_st_inx;
	uint32_t inode_data_bk_count;
	//���ݿ鿪ʼλ��
	uint32_t data_bk_st_inx;

	uint32_t last_alloc_bk_inx;
};
//sp_inode.c
extern struct super_operations sp_s_ops;
struct inode* sp_alloc_inode(struct inode *p_inode);
int32_t sp_free_inode(struct inode *p_inode);
void sp_write_inode(struct inode *i_node);
struct super_block* sp_alloc_sb(struct super_block *sb);
void sp_free_sb(struct super_block *sb);
struct super_block* sp_read_sb(struct super_block *sb);

struct inode* sp_new_inode(struct inode *p_inode);
int32_t get_bk_no_ofs(struct inode *p_inode, uint32_t offset, uint32_t *bk_no);

int sp_sync_inode(struct inode *inode);
int32_t get_bk_no_ofs(struct inode *p_inode, uint32_t offset, uint32_t *bk_no);

//sp_namei.c
int sp_lookup(struct inode *p_inode, const char *file_name, int len,
		struct inode **res_inode);
int sp_create(struct inode *dir, const char *name, int len, int mode,
		struct inode **result);
int sp_mknod(struct inode *dir, const char *name, int len, int mode, int rdev);
int sp_mkdir(struct inode *dir, const char *name, int len, int mode);
int sp_rmdir(struct inode *dir, const char *name, int len);
int sp_link(struct inode *oldinode, struct inode *dir, const char *name,
		int len);
int sys_unlink(const char *pathname);
int sp_symlink(struct inode *dir, const char *name, int len,
		const char *symname);
int sp_rename(struct inode *old_dir, const char *old_name, int old_len,
		struct inode *new_dir, const char *new_name, int new_len);
int sp_unlink(struct inode *dir, const char *name, int len);
//sp_super.c
int32_t sp_mkfs(dev_t dev_no, int32_t inode_count);

//sp_file.c
int sp_file_read(struct inode *inode, struct file *filp, char *buf, int count);
int sp_file_write(struct inode *inode, struct file *filp, char *buf, int count);

//dir.c
int sp_readdir(struct inode *inode, struct file *filp, struct dirent *dirent,
		int count);
int sp_dir_read(struct inode *inode, struct file *filp, char *buf, int count);

//sp_sync.c
int sp_sync_file(struct inode *inode, struct file *file);

//sp_file.c
extern struct inode_operations sp_file_inode_operations;
//sp_dir.c
extern struct inode_operations sp_dir_inode_operations;
//sp_rw.c
int32_t inode_alloc_new_bk(struct inode *inode, uint32_t *newBkNum);
int32_t get_ofs_bk_no(struct inode *inode, uint32_t offset, uint32_t *fpBkNum);

//sp_truncate.c
void sp_truncate(struct inode *inode, int32_t len);

//sp_link.c
extern struct inode_operations sp_symlink_inode_operations;

//sp_bmap.c
int32_t sp_set_bk_no_status(struct super_block *sb, uint32_t usedBkInx,
		uint8_t status);
int32_t sp_get_free_bk(struct super_block *sb);
int32_t sp_alloc_bk(struct super_block *sb, bk_no_t *res_bk);
int32_t sp_free_bk(struct super_block *sb, bk_no_t bk_no);
int32_t sp_get_free_inode(struct super_block *sb);
int32_t sp_alloc_inode_no(struct super_block *sb, ino_t *res_ino);
int32_t sp_free_inode_no(struct super_block *sb, ino_t ino);
int32_t sp_check_inode_no(struct super_block *sb, ino_t ino);
void sp_free_ct_bk(struct super_block *sb, uint32_t start_bk,uint32_t bk_num) ;
int32_t sp_alloc_ct_bk(struct super_block *sb, uint32_t need_bk_num,
		bk_no_t *res_sart_bk) ;
#endif //UNTITLED1_SP_H
