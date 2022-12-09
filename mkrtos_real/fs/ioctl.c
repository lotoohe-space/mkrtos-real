
#include <mkrtos/fs.h>
#include <mkrtos/sched.h>

//io¿ØÖÆ
int file_ioctl(int fd, uint32_t cmd, uint32_t arg){
    if(fd>=NR_FILE || get_current_task()->files[fd].used==0
    ){
        return  -EBADF;
    }
    struct file *_file=&(get_current_task()->files[fd]);

    if(!(_file->f_inode)
    ||!(_file->f_op)
    ||!(_file->f_op->ioctl)
    ){
        return -EINVAL;
    }else{
        int err =get_current_task()->files[fd].f_op->ioctl(_file->f_inode,_file,cmd,arg);
        if(err<0){
            return err;
        }
    }
    return 0;
}






