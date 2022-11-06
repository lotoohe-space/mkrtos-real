//
// Created by Administrator on 2022/1/21.
//

#include <mkrtos/fs.h>
#include <mkrtos/sp.h>
#include <mkrtos/bk.h>
#include <string.h>
#include <mkrtos/stat.h>
#include <knl_service.h>
//�ض��ļ���ָ����С
//��Ҫ���Ĺ�����������ضϺ�벿����ռ�õĿ�
void sp_truncate(struct inode* inode,int32_t len){
    struct super_block *sb;
    struct sp_super_block* sp_sb;
    struct sp_inode *sp_ino;
    uint32_t bk_ofs;
    uint32_t bk_end_ofs;
    if (!S_ISREG(inode->i_type_mode)) {
        return;
    }
    sb=inode->i_sb;
    sp_sb=sb->s_sb_priv_info;
    sp_ino=inode->i_fs_priv_info;

    bk_ofs=ROUND_UP(len,sb->s_bk_size);
    bk_end_ofs=ROUND_UP(inode->i_file_size,sb->s_bk_size);

    inode->i_file_size=len;

   for(uint32_t i = bk_ofs;i<bk_end_ofs;i++){
       if (i < A_BK_NUM(sp_ino)) {
            if(sp_ino->p_ino[i]!=0){
            	sp_free_bk(sb,sp_ino->p_ino[i]);
                sp_ino->p_ino[i]=0;
            }
       }
       else if (i < A_BK_NUM(sp_ino) + B_BK_NUM(sb,sp_ino)) {
            uint32_t bk_ofs=i- A_BK_NUM(sp_ino);
            uint32_t p_bk=bk_ofs/(512/4);
            uint32_t p_bk_ofs=bk_ofs%(512/4);
            struct bk_cache *bk_tmp;
            bk_tmp=bk_read(sb->s_dev_no,sp_ino->pp_ino[p_bk],1,0);
            uint32_t w;
            mkrtos_memcpy(&w,bk_tmp->cache+(p_bk_ofs<<2),4);
            sp_free_bk(sb,w);
            w=0;
            mkrtos_memcpy(bk_tmp->cache+(p_bk_ofs<<2),&w,4);
            bk_release(bk_tmp);
       }
//        else if (i < A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino) + C_BK_NUM(sb, sp_ino)) {
//
//        }
       else {
           //�����˵����ļ��Ĵ�С
           goto end;
       }
    }
end:
   for( int i=0;i<sizeof(sp_ino->pp_ino) / 4;i++) {
	   if (sp_ino->pp_ino[i]!=0) {
		   sp_free_bk(sb,sp_ino->pp_ino[i]);
	   }
   }

}
