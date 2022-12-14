/*
 * exec.c
 *
 *  Created on: 2022??9??5??
 *      Author: Administrator
 */

#include <type.h>
#include <assert.h>
#include <errno.h>
#include <knl_service.h>
#include <mkrtos/exec.h>
#include <arch/atomic.h>
#include <mkrtos.h>
#include <mkrtos/sched.h>
#include <fcntl.h>
#include <mkrtos/debug.h>

#define EXEC_FILE_NR 5

static struct exec_file_info *exec_file_list[EXEC_FILE_NR];
static struct atomic exec_file_atomic_ref[EXEC_FILE_NR];

int32_t exec_init(void) {
	for (int32_t i = 0; i < EXEC_FILE_NR; i++) {
		exec_file_atomic_ref[i] = ATOMIC_INIT(0);
	}
}
INIT_LV1(exec_init);

/**
 * @brief ???????exec
 */
int32_t exec_alloc(void) {
	for (int32_t i = 0; i < EXEC_FILE_NR; i++) {
		if (atomic_test_inc(&exec_file_atomic_ref[i])) {
			return i;
		}
	}
	return -1;
}
/**
 * @brief ????????
 */
struct exec_file_info *exec_get(int32_t i) {
	if (i < 0 || i >= EXEC_FILE_NR) {
		return 0;
	}
	atomic_inc(&exec_file_atomic_ref[i]);
	return exec_file_list[i];
}
/**
 * @brief ??????exec
 */
int32_t exec_put(int32_t i) {
	int ret;

	if (i < 0 || i >= EXEC_FILE_NR) {
		return -1;
	}
	ret = atomic_test_dec_nq(&exec_file_atomic_ref[i]);
	if (ret) {
		exec_file_list[i] = NULL;
	}
	return !ret;
}

/**
 * @brief ?args???????
 */
static int sys_args_alloc(char* argv[], int *ret)
{
	int need_len = 0;
	int argc_len = 0;

	if (!argv) {
		*ret = 0;
		return 0;
	}
	while (argv[argc_len]) {
		need_len += mkrtos_strlen(argv[argc_len]) + 1;
		argc_len++;
	}
	//?????????????
	*ret = need_len + ((argc_len + 2) << 2);
	return argc_len;
}
static void sys_args_set(char* argv[], int arg_len, int arg_data_len,uint8_t *arg_data)
{
	uint32_t *arg_data_tmp = (uint32_t*) arg_data;
	uint32_t arg_data_start_inx = 4 + (arg_len << 2);
	uint32_t argc_len = 0;
	uint32_t dat_tmp_inx = 0;

	if (!arg_len) {
		return;
	}

	arg_data_tmp[dat_tmp_inx++] = arg_len;
	for(int i=0;i < arg_len;i++) {
		mkrtos_strcpy((char*)(arg_data + arg_data_start_inx), argv[i]);
		arg_data_tmp[dat_tmp_inx++] = (uint32_t)(arg_data + arg_data_start_inx);
		arg_data_start_inx+=mkrtos_strlen(argv[i]) +1;
	}
	(*((uint32_t *)(arg_data + arg_data_start_inx)))=0;
}

int sys_do_execve(struct task *s_task, const char *filename, char *const argv[], char *const envp[])
{

	int fp;
	uint32_t start_addr;
	int ret;
	int32_t exec_id;
	struct exec_file_info *exec_fi;
	void *ram;
	struct task *curr_task;
	int argc_len = 0;
	int argc_need_len = 0;
	uint32_t t = 0;
	struct task *c_task = get_current_task();
	curr_task = s_task;

	argc_len = sys_args_alloc(argv, &argc_need_len);

	fp = sys_open(filename, O_RDONLY, 0777);
	if (fp < 0) {
		kprint("File open error.\n");
		return -ENOEXEC;
	} 
	ret = sys_ioctl(fp, 3, &start_addr);
	if (ret < 0) {
		sys_close(fp);
		DEBUG("exec", INFO, "exec read file error. ret %d.\ n", ret);
		return -1;
	}
	sys_close(fp);
	kprint("%s entry:0x%x.\n", filename, start_addr);

	t = dis_cpu_intr();

	if (curr_task->exec_id >= 0) {
		free_align(curr_task->user_ram);
	}

	exec_id = exec_alloc();
	if (exec_id < 0) {
		sys_close(fp);
		restore_cpu_intr(t);
		kprint("The maximum number of system executables has been reached.\n");
		return -1;
	}

	exec_fi = (struct exec_file_info*) start_addr;

	for(int i=0;i<sizeof(EXEC_MAGIC)-1;i++) {
		 if (exec_fi->magic[i]!=EXEC_MAGIC[i]) {
			exec_put(exec_id);
			sys_close(fp);
			restore_cpu_intr(t);
			kprint("Executable file format error.\n");
			return -ENOEXEC;
		 }
	}
	if (!curr_task->mpu) {
		curr_task->mpu = malloc(sizeof(mpu_t));
	}
	if (!curr_task->mpu) {
		exec_put(exec_id);
		sys_close(fp);
		restore_cpu_intr(t);
		kprint("The system is low on memory.\n");
		return -ENOMEM;
	}
	uint32_t pre_alloc_addr;
	struct mem_heap *heap = NULL;

again_alloc:
	heap = knl_mem_get_free(heap, exec_fi->i.ram_size + argc_need_len, &pre_alloc_addr);
	if (!heap) {
		exec_put(exec_id);
		if (curr_task->mpu) {
			free(curr_task->mpu);
		}
		kprint("The system is low on memory.\n");
		knl_mem_trace();
		restore_cpu_intr(t);
		return -ENOMEM;
	}

	int need_align;
	mpu_set(curr_task->mpu->mpu_info, (void *)pre_alloc_addr, exec_fi->i.ram_size + argc_need_len, &need_align);

	ram = malloc_align(exec_fi->i.ram_size + argc_need_len, need_align);
	if (!ram) {
		exec_put(exec_id);
		if (curr_task->mpu) {
			free(curr_task->mpu);
		}
		restore_cpu_intr(t);
		kprint("The system is low on memory.\n");
		return -ENOMEM;
	}
	if (ram != curr_task->mpu->mpu_info[0].start_addr) {
		kprint("Again.\n");
		free_align(ram);
		heap = heap->next;
		goto again_alloc;
	}

	kprint("%s data:0x%x. size:0x%x\n", filename, ram, exec_fi->i.ram_size + argc_need_len);

	if (curr_task->exec_id >= 0) {
		exec_put(curr_task->exec_id);
		for(int i=3;i<NR_FILE;i++){
			if (curr_task->files[i].used) {
				sys_close(i);
			}
		}
	} else {

		if (c_task->files[0].used) {
			curr_task->files[0] = get_current_task()->files[0];
			atomic_inc(&(curr_task->files[0].f_inode->i_used_count));
			curr_task->files[0].f_op->open(curr_task->files[0].f_inode,
					&curr_task->files[0]);
		} else {
			sys_open("/dev/tty0",O_RDWR,0777);
		}
		if (c_task->files[1].used) {
			curr_task->files[1] = get_current_task()->files[1];
			atomic_inc(&(curr_task->files[1].f_inode->i_used_count));
			curr_task->files[1].f_op->open(curr_task->files[1].f_inode,
								&curr_task->files[1]);
		} else {
			sys_open("/dev/tty0",O_RDWR,0777);
		}
		if (c_task->files[2].used) {
			curr_task->files[2] = get_current_task()->files[2];
			atomic_inc(&(curr_task->files[2].f_inode->i_used_count));
			curr_task->files[2].f_op->open(curr_task->files[2].f_inode,
								&curr_task->files[2]);
		} else {
			sys_open("/dev/tty0",O_RDWR,0777);
		}
		atomic_inc(&get_current_task()->root_inode->i_used_count);
		atomic_inc(&get_current_task()->pwd_inode->i_used_count);
		curr_task->root_inode = get_current_task()->root_inode;
		curr_task->pwd_inode = get_current_task()->pwd_inode;

	}

	curr_task->exec_id = exec_id;
	curr_task->user_ram = ram;
	curr_task->user_ram_size = exec_fi->i.ram_size + argc_need_len;
	exec_file_list[exec_id] = exec_fi;


	curr_task->mem_low_stack = ((uint32_t)ram + exec_fi->i.stack_offset - exec_fi->i.data_offset);
	curr_task->user_stack_size = (uint32_t)(exec_fi->i.stack_size) >> 2;

	curr_task->sk_info.user_stack =
			(ptr_t) (&(((uint32_t*) curr_task->mem_low_stack)[curr_task->user_stack_size
					- 1])) & (~7UL);


	uint8_t *arg_data;

	arg_data=(uint8_t *)ram + exec_fi->i.ram_size;
	sys_args_set(argv,argc_len,argc_need_len,arg_data);
	mkrtos_memset(ram, 0, exec_fi->i.ram_size);
	curr_task->sk_info.user_stack = (ptr_t)os_task_set_reg(
			curr_task->sk_info.user_stack, start_addr | 0x1, arg_data, 0, 0, 0, ram);
	curr_task->sk_info.knl_stack =
			(((ptr_t) (&(((uint32_t*) curr_task->knl_low_stack)[curr_task->kernel_stack_size - 1])))
					& (~0x7UL));

	curr_task->sk_info.stack_type = 1;
	curr_task->status = TASK_RUNNING;

	curr_task->sig_bmp[0] = 0;
	curr_task->sig_bmp[1] = 0;
	for (int i = 0; i < _NSIG; i++) {
		curr_task->signals[i]._u._sa_handler = SIG_DFL;
	}

	mem_init(&curr_task->user_head_alloc, 1);
	mem_heap_add(&curr_task->user_head_alloc, (char *)ram+exec_fi->i.heap_offset - exec_fi->i.data_offset,exec_fi->i.heap_size);

	if (curr_task == c_task) {
		get_sys_tasks_info()->is_first = FALSE;
		get_sys_tasks_info()->last_task = NULL;

		set_psp(0x0);
	}
	task_sche();
	restore_cpu_intr(t);
	DEBUG("fs",INFO,"%s text:0x%x data:0x%x\n",filename,exec_fi,ram);
	return 0;
}
int sys_execve(const char *filename, char *const argv[], char *const envp[])
{
	return sys_do_execve(get_current_task(), filename, argv, envp);
}
