/**
* @breif �ڴ����
* @author 1358745329@qq.com
*/

#include "arch/arch.h"
#include "errno.h"
#include "stdio.h"
#include <string.h>
#include <mkrtos/mem.h>
#include <mkrtos/task.h>

/**
* @breif �ڴ��
*/
mem_type __attribute__((aligned(MALLOC_BLOCK0_SIZE))) mem0[MALLOC_MEM0_SIZE]={0};
/**
* @breif �ڴ�����
*/
mem_tb_type mem0_man_tb[MALLOC_MANAGER0_TABLE]={0};


#ifndef NOT_USE_EX_MEM
uint8_t __attribute__((aligned(MALLOC_BLOCK0_SIZE))) malloc_ex_mem[MALLOC_EX_MEM_SIZE] __attribute__((section(".ext_sram")))={0};
mem_tb_type manager_ex_table[MALLOC_EX_MANAGER_TABLE] __attribute__((section(".ext_sram")))= { 0 };
#endif

/**
* @breif OS���ڴ������
*/
struct mem_item{
	
	/**
	* @breif �ڴ�
	*/
	mem_type* mem;
	/**
	* @breif �ڴ�����
	*/
	mem_tb_type* mem_man_tb;
	/**
	* @breif �ڴ��С
	*/
	uint32_t mem_size;
	/**
	* @breif �ڴ������С
	*/
	uint32_t mem_man_tb_size;
	/**
	* @breif ���С
	*/
	uint16_t mem_block_size;
	
	/**
	* @breif ʣ��Ŀ�����
	*/
	uint32_t free_block_num;
	
};


struct mem{
	
	/**
	* @breif �ڴ������
	*/
	struct mem_item mem_item_ls[MEM_AREA_NUM];
	
	/**
	* @breif �ڴ�������
	*/
	uint16_t mem_num;
	
};

static int mem_init_flag=0;
/**
* @breif �ڴ���������ʼ��
*/
struct mem sys_mem={
	.mem_num=MEM_AREA_NUM,
	.mem_item_ls[0].mem=mem0,
	.mem_item_ls[0].mem_man_tb=mem0_man_tb,
	.mem_item_ls[0].mem_size=MALLOC_MEM0_SIZE,
	.mem_item_ls[0].mem_man_tb_size=MALLOC_MANAGER0_TABLE,
	.mem_item_ls[0].mem_block_size=MALLOC_BLOCK0_SIZE,
	.mem_item_ls[0].free_block_num = MALLOC_MANAGER0_TABLE,
	#ifndef NOT_USE_EX_MEM
	.mem_item_ls[1].mem=malloc_ex_mem,
	.mem_item_ls[1].mem_man_tb=manager_ex_table,
	.mem_item_ls[1].mem_size=MALLOC_EX_MEM_SIZE,
	.mem_item_ls[1].mem_man_tb_size=MALLOC_EX_MANAGER_TABLE,
	.mem_item_ls[1].mem_block_size=MALLOC_EX_BLOCK_SIZE,
	.mem_item_ls[1].free_block_num = MALLOC_EX_MANAGER_TABLE,
	#endif
};



/**
* @breif ��ʼ���ڴ����
*/
void init_mem(void) {
    if(mem_init_flag){
        return ;
    }
	/*������е��ڴ������*/
	uint16_t i;
	for (i = 0; i < sys_mem.mem_num; i++) {
		uint32_t j=0;
		for(j=0;j<sys_mem.mem_item_ls[i].mem_man_tb_size;j++){
			sys_mem.mem_item_ls[i].mem_man_tb[j]=0;
		}
		//memset(sys_mem.mem_item_ls[i].mem_man_tb, 0, sys_mem.mem_item_ls[i].mem_man_tb_size * sizeof(mem_tb_type));
	}
    mem_init_flag=1;
}
/**
* @breif �����ڴ�
* @param inxMem ����һ���ڴ��������
* @param size ��Ҫ������ڴ��С
*/
void* _malloc(uint16_t inxMem, uint32_t size) {
	uint32_t i_need_block_num;//��Ҫ��block����
	uint32_t i;
	uint32_t find_block_num = 0;//�ҵ��ĿյĿ�
	uint32_t temp;
	uint8_t flag = 0;
	uint16_t bkSize;
	mem_tb_type* manager_table;
	mem_type* malloc_mem;
    init_mem();
	if (inxMem >= sys_mem.mem_num) {
		/*��������*/
		return NULL;
	}
	malloc_mem = sys_mem.mem_item_ls[inxMem].mem;
	manager_table = sys_mem.mem_item_ls[inxMem].mem_man_tb;
	bkSize = sys_mem.mem_item_ls[inxMem].mem_block_size;

	i_need_block_num = size / bkSize + ((size % bkSize == 0) ? 0 : 1);

	for (i = 0; i < sys_mem.mem_item_ls[inxMem].mem_man_tb_size;) {
		if (manager_table[i] == 0) {
			find_block_num++;
			if (find_block_num == i_need_block_num) {
				flag = 1;
				break;
			}
			i++;
		}
		else {
			find_block_num = 0;
			i += manager_table[i];
		}
	}
	if (flag != 1 || i >= sys_mem.mem_item_ls[inxMem].mem_man_tb_size) {//û���ҵ������߳�����
		return NULL;
	}
	i -= i_need_block_num - 1;
	for (temp = i; temp < i + i_need_block_num; temp++) {
		if (temp == i) {
			manager_table[i] = i_need_block_num;
		}
		else {
			manager_table[temp] = 1;
		}
	}
	sys_mem.mem_item_ls[inxMem].free_block_num -= i_need_block_num;
//    printk("malloc %x.\n\n",(void*)(&(malloc_mem[bkSize * i])));
    return (void*)(&(malloc_mem[bkSize * i]));
}
/**
* @brief �ͷ�������ڴ�
* @param inxMem ����һ���ڴ����ͷ�
* @param mem_addr �ͷŵ��ڴ��׵�ַ
*/
int32_t _free(uint16_t inxMem, void* mem_addr) {
    uint32_t i;
    if (mem_addr == NULL) { return -2; }
    InitMem();
	uint32_t free_size;
	uint16_t bkSize;
	mem_tb_type* manager_table;
	mem_type* malloc_mem;
	uint32_t i_mem_offset;
	uint32_t i_manager_offset;

    if(mem_addr<sys_mem.mem_item_ls[inxMem].mem
        ||mem_addr>(sys_mem.mem_item_ls[inxMem].mem+sys_mem.mem_item_ls[inxMem].mem_size)){
        return -1;
    }

	malloc_mem = sys_mem.mem_item_ls[inxMem].mem;
	manager_table = sys_mem.mem_item_ls[inxMem].mem_man_tb;
	bkSize = sys_mem.mem_item_ls[inxMem].mem_block_size;

	i_mem_offset = (uint32_t)mem_addr - (uint32_t)malloc_mem;
	i_manager_offset = i_mem_offset / bkSize;
	if (i_manager_offset > sys_mem.mem_item_ls[inxMem].mem_man_tb_size) {
		return -1;
	}
	
	sys_mem.mem_item_ls[inxMem].free_block_num += manager_table[i_manager_offset];

	free_size = manager_table[i_manager_offset];
	for (i = i_manager_offset; i < free_size + i_manager_offset; i++) {
		manager_table[i] = 0;
	}
    return 0;
}
uint32_t get_mem_size(uint16_t inxMem, void* mem_addr){
	if (mem_addr == NULL) { return 0; }
    InitMem();
	uint32_t free_size;
	uint16_t bkSize;
	mem_tb_type* manager_table;
	mem_type* malloc_mem;
	uint32_t i_mem_offset;
	uint32_t i_manager_offset;
	malloc_mem = sys_mem.mem_item_ls[inxMem].mem;
	manager_table = sys_mem.mem_item_ls[inxMem].mem_man_tb;
	bkSize = sys_mem.mem_item_ls[inxMem].mem_block_size;

	i_mem_offset = (uint32_t)mem_addr - (uint32_t)malloc_mem;
	i_manager_offset = i_mem_offset / bkSize;
	uint32_t i;
	if (i_manager_offset > sys_mem.mem_item_ls[inxMem].mem_man_tb_size) {
		return 0;
	}
	
	return manager_table[i_manager_offset]*bkSize;
}
/**
* @breif ��ȡʣ����ڴ�
* @param inxMem �ڴ������
* @return ����ʣ������ֽ�
*/
uint32_t get_free_memory(uint8_t mem_no) {
    init_mem();
	/*ʣ���ڴ��С*/
	return sys_mem.mem_item_ls[mem_no].free_block_num * sys_mem.mem_item_ls[OS_USE_MEM_AREA_INX].mem_block_size;

}
uint32_t get_total_memory(uint8_t mem_no){
	init_mem();
	return sys_mem.mem_item_ls[mem_no].mem_size;
}
/**
* @breif �����ڴ�
* @param size ����Ĵ�С
* @return �������뵽���ڴ棬ʧ���򷵻�NULL
*/
void* os_malloc(uint32_t size) {
	int32_t st=dis_cpu_intr();
	void* res = _malloc(0, size);
    if(!res){
        res=_malloc(1,size);
    }
	restore_cpu_intr(st);
//    if(res) {
//        memset(res, 0, size);
//    }
	return res;
}
#ifndef NOT_USE_EX_MEM
void* os_malloc_ex(uint32_t size) {
	int32_t st=dis_cpu_intr();
	void* res = _malloc(OS_USE_MEM_AREA_INX1, size);
	restore_cpu_intr(st);
	return res;
}
void os_free_ex(void* mem) {
	int32_t st=dis_cpu_intr();
	_free(OS_USE_MEM_AREA_INX1, mem);
	restore_cpu_intr(st);
}
void *os_realloc_ex(void* mem,uint32_t size){
	int32_t st=dis_cpu_intr();
	void* res = _malloc(OS_USE_MEM_AREA_INX1, size);
	if(res==NULL){
		restore_cpu_intr(st);
		return NULL;
	}
	memcpy(res,mem,get_mem_size(OS_USE_MEM_AREA_INX1,mem));
	os_free(mem);
	restore_cpu_intr(st);
	return res;
}
#endif
void *os_realloc(void *mem,uint32_t size){
	int32_t st=dis_cpu_intr();
	void* res = os_malloc(size);
	if(!res){
		restore_cpu_intr(st);
		return NULL;
	}
	memcpy(res,mem,get_mem_size(OS_USE_MEM_AREA_INX,mem));
	os_free(mem);
	restore_cpu_intr(st);
	return res;
}

/**
* @breif �ͷ�������ڴ�
* @param mem �ͷŵ��ڴ���׵�ַ
*/
void os_free(void* mem) {
	int32_t st;
    int ret;

    if(!mem){
        return ;
    }
//    printk("free %x.\n\n",mem);
    st=dis_cpu_intr();
    ret=_free(0, mem);
    if(ret<0){
        _free(1, mem);
    }
	restore_cpu_intr(st);
}

static int32_t mem_add(void *mem,int32_t lenght,struct inode *inode,int ofst){
    struct mem_struct* tmp;
    tmp= os_malloc(sizeof(struct mem_struct));
    if(tmp==NULL){
        return -1;
    }
    tmp->length=lenght;
    tmp->ofst=ofst;
    tmp->mem_start=mem;
    tmp->inode=inode;
    tmp->next=NULL;
    uint32_t  t;
    t=dis_cpu_intr();
    if(!CUR_TASK->mems){
        CUR_TASK->mems=tmp;
    }else{
        tmp->next=CUR_TASK->mems;
        CUR_TASK->mems=tmp;
    }
    restore_cpu_intr(t);
    return 0;
}
static void mem_remove(void *mem,int length){
    struct mem_struct *tmp;
    tmp=CUR_TASK->mems;
    struct mem_struct *prev=NULL;
    uint32_t t;
    t=dis_cpu_intr();
    while(tmp){
        if(tmp->mem_start ==mem) {
            if (prev==NULL) {
                //ɾ���ĵ�һ��
                CUR_TASK->mems=tmp->next;
                os_free(tmp);
                break;
            }else{
                prev->next=tmp->next;
                break;
            }
        }
        prev=tmp;
        tmp=tmp->next;
    }
    restore_cpu_intr(t);
}
struct mem_struct * mem_get(void *start){
    struct mem_struct *tmp;
    tmp=CUR_TASK->mems;
    uint32_t t;
    t=dis_cpu_intr();
    while(tmp){
        if(tmp->mem_start ==start) {
            restore_cpu_intr(t);
            return tmp;
        }
        tmp=tmp->next;
    }
    restore_cpu_intr(t);
    return NULL;
}
/**
 * �������ռ�õ��ڴ棬�Ñ���exit�������{��
 */
void mem_clear(void){
    uint32_t t;
    struct mem_struct **tmp;
   // t=dis_cpu_intr();
    tmp=&CUR_TASK->mems;
    while(*tmp){
        struct mem_struct *next;
        next=(*tmp)->next;

        struct mem_struct *rel_mem;

        rel_mem=(*tmp);
        if(rel_mem->inode){
            if(rel_mem->inode->i_ops
               &&rel_mem->inode->i_ops->default_file_ops->mumap
                    ){
                rel_mem->inode->i_ops->default_file_ops->mumap(rel_mem->inode,rel_mem->mem_start,rel_mem->length);
                atomic_dec(&rel_mem->inode->i_used_count);
            }
        }
        os_free(rel_mem->mem_start);
        os_free(rel_mem);
        *tmp=next;
    }
    CUR_TASK->mems=NULL;
  //  restore_cpu_intr(t);
}
void* sys_mmap(void *start, size_t length, int prot, int flags,int fd, off_t offset){
    struct inode *inode=NULL;
    int alloc_len=length;
    if(start){
        return -1;
    }
    if(fd!=-1) {
        if (fd < 0 || fd >= NR_FILE) {
            printk("%s fp.\n", __FUNCTION__);
            return -EBADF;
        }
        if (CUR_TASK->files[fd].used == 0) {
            return -EINVAL;
        }
        inode = CUR_TASK->files[fd].f_inode;
        struct super_block  *sb;
        sb= CUR_TASK->files[fd].f_inode->i_sb;
        //������Ҫ����ĳ���
        alloc_len=(
                length/sb->s_bk_size+((length%sb->s_bk_size)?1:0)
                + ((offset%sb->s_bk_size)?1:0)//offset���û�иպö���bksize������Ҫ���һ��
                )*sb->s_bk_size;
    }

    void* res_mem = os_malloc(alloc_len);
    if(!res_mem){
        return -1;
    }
    if(mem_add(res_mem,length,inode,offset)<0){
        os_free(res_mem);
        return -ENOMEM;
    }
//    printk("mmap %x start:%x length:%d prot:%d flags:%x fd:%d ost:%d\r\n",res_mem,start,length,prot,flags,fd,offset);
    if(fd>=0) {
        if (inode->i_ops &&
            inode->i_ops->default_file_ops->mmap
                ) {
            int res = inode->i_ops->default_file_ops->mmap(inode, CUR_TASK->files + fd, res_mem, alloc_len, 0, offset);
            if (res < 0) {
                mem_remove(res_mem,length);
                return res;
            }
            //mmapһ�Σ����ü���������һ��
            //�û���mmap�󣬾Ϳ��Ե��ùر�����ļ���
            atomic_inc(&inode->i_used_count);
        }

    }

    return res_mem;
}
void sys_munmap(void *start, size_t length){
    struct mem_struct *mem;
    mem=mem_get(start);
    if(!mem){
        return ;
    }
    if(mem->inode){
        if(mem->inode->i_ops
        &&mem->inode->i_ops->default_file_ops->mumap
        ){
            mem->inode->i_ops->default_file_ops->mumap(mem->inode,start,length);
           puti(mem->inode);
        }
    }
    mem_remove(start,length);

    os_free(start);
}
void *sys_mremap(void *__addr, size_t __old_len, size_t __new_len,
                 unsigned long __may_move){
    struct mem_struct *mem;
    mem=mem_get(__addr);
    if(!mem){
        return NULL;
    }
    if(mem->inode){
        void* newmem= os_malloc(__new_len);
        if(newmem==NULL){
            return NULL;
        }
        //���ͷ��ļ�ӳ��
        if(mem->inode->i_ops
           &&mem->inode->i_ops->default_file_ops->mumap
                ){
            mem->inode->i_ops->default_file_ops->mumap(mem->inode,__addr,mem->length);
        }else{
            os_free(newmem);
            return NULL;
        }

        //����ӳ��
        if (mem->inode->i_ops &&
                mem->inode->i_ops->default_file_ops->mmap
                ) {
            int res = mem->inode->i_ops->default_file_ops->mmap(mem->inode, NULL, (unsigned long)newmem, __new_len, 0, mem->ofst);
            if (res < 0) {
                os_free(newmem);
                return NULL;
            }
        }
        os_free(mem->mem_start);
        mem->mem_start=newmem;
        mem->length=__new_len;
        return newmem;
    }else{
        //����Ҫ����ӳ���ļ�
//        mem_remove(__addr,__old_len);
        void* newmem= os_realloc(__addr,__new_len);
        if(newmem!=NULL){
            mem->length=__new_len;
        }
        return newmem;
    }
}
