//
// Created by Administrator on 2022/1/10.
//

#include <mkrtos/fs.h>
#include <errno.h>
#include <mkrtos/sched.h>

//��Ŀ¼
int sys_readdir(unsigned int fd, struct dirent * dirent, uint32_t count){
    int32_t err;

    if(fd>=NR_FILE){
        return  -EBADF;
    }
    if(get_current_task()->files[fd].used==0){
        return -1;
    }
    struct file *_file=&(get_current_task()->files[fd]);
    if(_file->f_inode==0){
        return -EBADF;
    }
    if(_file->f_op
    &&_file->f_op->readdir){
        err=_file->f_op->readdir(_file->f_inode,_file,dirent,count);
    }else{
        return -ENOENT;
    }

    return err;
}
int sys_getdents(unsigned int fd, struct dirent *dirp, unsigned int count){
    int ret;
//    printk("%s.\n",__FUNCTION__ );
    ret=sys_readdir(fd, dirp,  count);
    return ret;
}
int sys_lseek(unsigned int fd, int32_t ofs, uint32_t origin){
    int32_t ofs_temp=0;
    if(fd>=NR_FILE){
        return  -EBADF;
    }
    if(get_current_task()->files[fd].used==0){
        return -1;
    }
    if(!get_current_task()->files[fd].f_inode){
        return -1;
    }
    if(get_current_task()->files[fd].f_op&&get_current_task()->files[fd].f_op->release) {
    	get_current_task()->files[fd].f_op->release(get_current_task()->files[fd].f_inode, &(get_current_task()->files[fd]));
    }
    ofs_temp=get_current_task()->files[fd].f_ofs;

    switch (origin) {
        case 0:
            ofs_temp = ofs;
            break;
        case 1:
            ofs_temp += ofs;
            break;
        case 2:
            if(!(get_current_task()->files[fd].f_inode)){
                return -1;
            }
            ofs_temp = get_current_task()->files[fd].f_inode->i_file_size + ofs;
            break;
    }
    if(ofs_temp<0){
        return -1;
    }
    get_current_task()->files[fd].f_ofs=ofs_temp;
    return get_current_task()->files[fd].f_ofs;
}
int file_read (int fd,uint8_t *buf,uint32_t len){
    if(fd>=NR_FILE){
        return  -EBADF;
    }
    if(get_current_task()->files[fd].used==0){
        return -1;
    }
    struct file *_file=&(get_current_task()->files[fd]);

    if(!(_file->f_inode)){
        return -EBADF;
    }

    if(_file->f_op
    &&_file->f_op->read
    ){
        int32_t err=_file->f_op->read(_file->f_inode,_file,buf,len);
        return err;
    }else{
        return -EINVAL;
    }

    return -ENOSYS;
}

int file_write (int fd,uint8_t *buf,uint32_t len){
    if(fd>=NR_FILE){
        return -EBADF;
    }
    if(get_current_task()->files[fd].used==0){
        return -1;
    }
    struct file *_file=&(get_current_task()->files[fd]);

    if(!(_file->f_inode)){
        return -EBADF;
    }

    if(_file->f_op
       &&_file->f_op->write
            ){
        int32_t err=_file->f_op->write(_file->f_inode,_file,buf,len);
        return err;
    }
    return -EINVAL;
}

