#include <mkrtos/fs.h>
#include <mkrtos/cpio.h>
#include <mkrtos/bk.h>
#include <mkrtos/debug.h>
#include <knl_service.h>
static int64_t check_magic(char magic[6])
{
	if (magic[0] == '0' && magic[1] == '7' && magic[2] == '0' &&
		magic[3] == '7' && magic[4] == '0' && magic[5] == '1') {
			return 0;
	}
	return -1;
}
int32_t cpio_read_data(struct super_block *sb, uint8_t *data, uint16_t len, int32_t r_offs)
{
	int rs_bk = ROUND_DOWN(r_offs, sb->s_bk_size);
	cpio_sb_t *sp_sb = sb->s_sb_priv_info;
	int read_len = 0;

	for (int i=rs_bk;read_len < len;i++) {
		bk_cache_t *bk_tmp;
		int once_read_len = 0;
		int bk_use_size;
		bk_tmp = bk_read(sb->s_dev_no, i, 0, 0);

		once_read_len = MIN(sb->s_bk_size, len - read_len);
		bk_use_size = (r_offs % sb->s_bk_size);
		once_read_len = MIN(once_read_len, sb->s_bk_size - bk_use_size);

		mkrtos_memcpy(data + read_len, bk_tmp->cache + bk_use_size, once_read_len);

		read_len += once_read_len;
		bk_release(bk_tmp);
		r_offs = sb->s_bk_size;
	}
	return read_len;
}
int32_t cpio_root_file_count(struct super_block *sb, cpio_fs_ino_priv_t *cpio_info_fs, uint32_t *count)
{
	uint8_t rByte;
	cpio_sb_t *sp_sb = sb->s_sb_priv_info;
	int32_t bk_inx;
	int f_count = 0;

	bk_cache_t *bk_tmp;
	int offs = 0;
	int ret = 0;
	char f_name[32];
	if (!cpio_info_fs) {
		return -1;
	}
	
	for (int i=0;;i++) {
		if (cpio_read_data(sb, &cpio_info_fs->fs_info, sizeof(cpio_fs_t), offs)
			!=sizeof(cpio_fs_t)) {
				ret = -1;
				goto end;
		}
		if (check_magic(cpio_info_fs->fs_info.c_magic) < 0) {
			ret = -1;
			goto end;
		}
		if (cpio_read_data(sb, f_name, MIN(htoi(cpio_info_fs->fs_info.c_namesize, 8), sizeof(f_name)), offs + sizeof(cpio_fs_t))<=0) {
				ret = -1;
			goto end;
		}

		if (mkrtos_strcmp("TRAILER!!!", f_name) == 0) {
			goto end;
		}
		f_count++ ;
		offs += ALIGN(sizeof(cpio_fs_t) +  htoi(cpio_info_fs->fs_info.c_namesize, 8) +  ALIGN(htoi(cpio_info_fs->fs_info.c_filesize, 8),4),4);
	}

	end:
	*count = f_count;
	return ret;
}
int32_t cpio_find_file(struct super_block *sb, const char *name, cpio_fs_ino_priv_t *cpio_info_fs)
{
	uint8_t rByte;
	cpio_sb_t *sp_sb = sb->s_sb_priv_info;
	int32_t bk_inx;

	bk_cache_t *bk_tmp;
	int offs = 0;
	char f_name[32];
	if (!cpio_info_fs) {
		return -1;
	}
	
	for (int i=0;;i++) {
		if (cpio_read_data(sb, &cpio_info_fs->fs_info, sizeof(cpio_fs_t), offs)
			!=sizeof(cpio_fs_t)) {
				return -1;
		}
		if (check_magic(cpio_info_fs->fs_info.c_magic) < 0) {
			return -1;
		}
		if (cpio_read_data(sb, f_name, MIN(htoi(cpio_info_fs->fs_info.c_namesize, 8), sizeof(f_name)),
				 offs + sizeof(cpio_fs_t))<=0) {
				return -1;
		}

		if (mkrtos_strcmp(f_name, name) == 0) {
			return 0;
		}

		if (mkrtos_strcmp("TRAILER!!", f_name) == 0) {
			return -1;
		}
		offs = ALIGN( ALIGN(offs + sizeof(cpio_fs_t) +  htoi(cpio_info_fs->fs_info.c_namesize, 8) ,4) + 
		 htoi(cpio_info_fs->fs_info.c_filesize, 8), 4);
	}
	return 0;
}

int htoi(char* str, int len)
{
	int n = 0;
	int i = 0;

	if (str == NULL || len <= 0) return -1;
	for(i=0;i<len;i++){
		if(str[i]!='0'){
			break;
		}
	}
	str+=i;
	while (1)
	{
		if (*str >= '0' && *str <= '9')
		{
			n = 16 * n + (*str - '0');
		}
		else if (*str >= 'A' && *str <= 'F')//十六进制还要判断字符是不是在A-F或者a-f之间
		{
			n = 16 * n + (*str - 'A' + 10);
		}
		else if (*str >= 'a' && *str <= 'f')
		{
			n = 16 * n + (*str - 'a' + 10);
		}
		else
		{
			break;
		}
		str++;
		i++;
		if (i>=len) {
			break;
		}
	}
	return n;
}
/**
 * 检查磁盘上的inode是否存在
 * @param sb
 * @param ino
 * @return 1：找到 0未找到
 */
int32_t cpio_check_inode_no(struct super_block *sb, ino_t ino, cpio_fs_ino_priv_t *cpio_info_fs) {
	uint8_t rByte;
	cpio_sb_t *sp_sb = sb->s_sb_priv_info;
	int32_t bk_inx;

	bk_cache_t *bk_tmp;
	int offs = 0;
	char f_name[32];
	if (!cpio_info_fs) {
		return -1;
	}
	
	for (int i=0;;i++) {
		if (cpio_read_data(sb, &cpio_info_fs->fs_info, sizeof(cpio_fs_t), offs)
			!=sizeof(cpio_fs_t)) {
				return -1;
		}
		if (check_magic(cpio_info_fs->fs_info.c_magic) < 0) {
			return -1;
		}

		if (cpio_read_data(sb, f_name, MIN(htoi(cpio_info_fs->fs_info.c_namesize, 8), sizeof(f_name)), offs + sizeof(cpio_fs_t))<=0) {
				return -1;
		}

		if (mkrtos_strcmp("TRAILER!!", f_name) == 0) {
			return -1;
		}
		if (htoi(cpio_info_fs->fs_info.c_ino, 8) == ino) {
			cpio_info_fs->st_bk = i;
			cpio_info_fs->in_bk_offs = offs + ALIGN(sizeof(cpio_fs_t) + htoi(cpio_info_fs->fs_info.c_namesize, 8), 4);
			return 1;
		}
		offs += ALIGN(sizeof(cpio_fs_t) +  htoi(cpio_info_fs->fs_info.c_namesize, 8) +  ALIGN(htoi(cpio_info_fs->fs_info.c_filesize, 8),4),4);
	}
	return 0;
}

