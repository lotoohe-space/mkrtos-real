//
// Created by Administrator on 2022/1/16.
//
#include <mkrtos/fs.h>
#include <string.h>
#include <mkrtos/sp.h>
#include <mkrtos/cpio.h>
/**
 * 文件系统的类型列表
 */
struct fs_type fs_type_list[]={
	{
		.f_name="sp",
		.req_dev_no=1,
		.alloc_sb=sp_alloc_sb,
		.free_sb=sp_free_sb,
		.read_sb=sp_read_sb
	},
    {
		.f_name="cpio",
		// .req_dev_no=1,
		.alloc_sb=NULL,
		.free_sb=NULL,
		.read_sb=cpio_read_sb
	}
};

int32_t fs_type_len=sizeof(fs_type_list)/sizeof(struct fs_type);

struct fs_type *find_fs_type(const char *fs_name){
    int32_t i;
    for(i=0;i<fs_type_len;i++){
        if(mkrtos_strcmp(fs_type_list[i].f_name,fs_name)==0){
            return &(fs_type_list[i]);
        }
    }
    return NULL;
}
