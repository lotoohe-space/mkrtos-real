//
// Created by Administrator on 2022/1/16.
//
#include <type.h>
#include <mkrtos/fs.h>
#include <mkrtos/sp.h>
#include <knl_service.h>
#include <mkrtos/bk.h>
#include <string.h>
#include <mkrtos/debug.h>
#include <knl_service.h>
#include <sys/stat.h>
/**
 * sp��sb����
 * @param sb
 * @return
 */
struct super_block* sp_alloc_sb(struct super_block *sb) {
	sb->s_sb_priv_info = malloc(sizeof(struct sp_super_block));
	if (sb->s_sb_priv_info == NULL) {
		return NULL;
	}
	mkrtos_memset(sb->s_sb_priv_info, 0, sizeof(struct sp_super_block));
	return sb;
}
void sp_free_sb(struct super_block *sb) {
	if (sb->s_sb_priv_info == NULL) {
		return ;
	}
	free(sb->s_sb_priv_info);
}

/**
 * ��ʽ��sp�ļ�ϵͳ
 * @param dev_no
 * @param inode_num
 * @return
 */
int32_t sp_mkfs(dev_t dev_no, int32_t inode_count) {
	bk_no_t new_bk;
	ino_t new_inode;
	int32_t res = 0;
	struct sp_super_block *sp_sb;
	struct super_block *sb;
	sb = malloc(sizeof(struct super_block));
	sb->s_sb_priv_info = malloc(sizeof(struct sp_super_block));
	sp_sb = sb->s_sb_priv_info;

	sb->s_dirt = 1;
	sb->s_ops = &sp_s_ops;
	sb->s_dev_no = dev_no;
	sp_sb->inode_count = sb->s_inode_size = 128;
	sb->s_bk_size = get_bk_size(dev_no);

	sp_sb->block_count = get_bk_count(dev_no);
	//super_block��ʼ�Ŀ�λ��
	sp_sb->fs_info_st_bk_inx = 1;

	if (inode_count <= 0) {
		//�Զ�inode����Ϊ30%�Ŀ���
		sp_sb->inode_count = (get_bk_count(dev_no) * 3) / 10;
	} else {
		sp_sb->inode_count = inode_count;
	}

	//iNodeͳ��ʹ����Ҫ���ٿ�
	uint32_t iNodeUsedBkCn;
	iNodeUsedBkCn = (sp_sb->inode_count / 8 / sp_sb->inode_size)
			+ (((sp_sb->inode_count / 8) + (sp_sb->inode_count % 8) ? 1 : 0)
					/ sp_sb->inode_size
					+ (((sp_sb->inode_count / 8) + (sp_sb->inode_count % 8) ?
							1 : 0) % sp_sb->inode_size) ? 1 : 0);
	sp_sb->inode_used_bk_count = iNodeUsedBkCn;
	sp_sb->inode_used_bk_st_inx = sp_sb->fs_info_st_bk_inx + 1;

	//BKͳ��ʹ����Ҫ���ٿ�
	uint32_t bkUsedBkCn;
	bkUsedBkCn = (sp_sb->block_count / 8 / sb->s_bk_size)
			+ (((sp_sb->block_count / 8) + (sp_sb->block_count % 8) ? 1 : 0)
					/ sb->s_bk_size
					+ (((sp_sb->block_count / 8) + (sp_sb->block_count % 8) ?
							1 : 0) % sb->s_bk_size) ? 1 : 0);
	sp_sb->bk_used_bk_count = bkUsedBkCn;
	sp_sb->bk_used_bk_st_inx = sp_sb->inode_used_bk_count
			+ sp_sb->inode_used_bk_st_inx;

	uint8_t *temp_mem = malloc((sb->s_bk_size));
	mkrtos_memset(temp_mem, 0xff, sb->s_bk_size);
	//дiNodeʹ�ÿ���Ϣ
	for (uint32_t i = 0; i < iNodeUsedBkCn; i++) {
		wbk(sb->s_dev_no, sp_sb->inode_used_bk_st_inx + i, temp_mem, 0,
				sb->s_bk_size);
	}

	//дbkʹ�ÿ���Ϣ
	for (uint32_t i = 0; i < bkUsedBkCn; i++) {
		wbk(sb->s_dev_no, sp_sb->bk_used_bk_st_inx + i, temp_mem, 0, sb->s_bk_size);
	}

	//����iNode��ռ�õ�����
	uint32_t eCn;
	eCn =
			(sp_sb->inode_count * sb->s_inode_size) / sb->s_bk_size
					+ (((sp_sb->inode_count * sb->s_inode_size) % sb->s_bk_size) ?
							1 : 0);

	sp_sb->inode_data_bk_st_inx = sp_sb->bk_used_bk_st_inx + sp_sb->bk_used_bk_count;
	sp_sb->inode_data_bk_count = eCn;

	//���ݵĿ�ʼλ��
	sp_sb->data_bk_st_inx = sp_sb->inode_data_bk_st_inx + sp_sb->inode_data_bk_count;
	sp_sb->last_alloc_bk_inx = 0;
	//дFSInfo��FS_INFO_ST_BK_INX��
	sb->s_magic_num = SP_MAGIC_NUM;

	sp_write_super(sb);

	//��Ǳ�FSĬ��ʹ�õĿ�
	for (uint32_t i = 0; i < sp_sb->data_bk_st_inx; i++) {
		sp_set_bk_no_status(sb, i, 1);
	}

	sb->s_dev_no = dev_no;

	//�½�. .. �ļ������Ҷ�ָ���Լ�
	struct dir_item di;
	struct inode *r_inode;
	struct sp_inode *sp_ino;
	if (sp_alloc_bk(sb, &new_bk) < 0) {
		res = -1;
		goto end;
	}
	if (sp_alloc_inode_no(sb, &new_inode) < 0) {
		sp_free_bk(sb, new_bk);
		res = -1;
		goto end;
	}
	r_inode = malloc(sizeof(struct inode));
	if (r_inode == NULL) {
		res = -1;
		//����ʧ��
		goto end1;
	}
	r_inode->i_fs_priv_info = NULL;
	//������
	r_inode->i_file_size = sizeof(struct dir_item) * 2;
	//����ΪĿ¼
	r_inode->i_type_mode = MK_MODE(S_IFDIR,0777);
	//����sb
	r_inode->i_sb = sb;
	r_inode->i_dirt = 1;
	r_inode->i_no = new_inode;
	//Ӳ����������2
	r_inode->i_hlink = 2;
	atomic_set(&(r_inode->i_used_count), 1);
	//����һ��inode
	if (sp_alloc_inode(r_inode) == NULL) {
		res = -1;
		//����ʧ��
		goto end2;
	}

	sp_ino = r_inode->i_fs_priv_info;
	//����µ�bk��
	sp_ino->p_ino[0] = new_bk;
	//��Ŀ¼�µ�. .. Ŀ¼
	mkrtos_strcpy(di.name, ".");
	di.used = TRUE;
	di.inode_no = 0;

	if (wbk(sb->s_dev_no, new_bk, (uint8_t*) (&di), 0, sizeof(di)) < 0) {
		res = -1;
		goto end2;
	}
	mkrtos_strcpy(di.name, "..");
	di.used = TRUE;
	di.inode_no = 0;

	if (wbk(sb->s_dev_no, new_bk, (uint8_t*) (&di), sizeof(di), sizeof(di))
			< 0) {
		res = -1;
		goto end2;
	}
	//дinode
	sp_write_inode(r_inode);
	//ͬ�����еĿ�
	sync_all_bk(sb->s_dev_no);

	free(r_inode);
	goto end;
	end2: sp_free_inode(r_inode);
	free(r_inode);
	end1: sp_free_bk(sb, new_bk);
	sp_free_bk(sb, new_inode);
	end:
	free(sb->s_sb_priv_info);
	free(sb);
	return 0;
}
