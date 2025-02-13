
#include <errno.h>
#include <rpc_prot.h>
#include <string.h>
#include <u_hd_man.h>
#include <u_prot.h>
#include <u_rpc_buf.h>
#include <u_rpc_svr.h>
#include <u_task.h>
#include <u_thread.h>
#include <u_types.h>

#define MAGIC_NS_USERPID 0xbabababa

int fast_ipc_setsp(int i, void *stack);

#define FAST_IPC_MAIN_STACK_SIZE 512
static ATTR_ALIGN(8) uint8_t com_stack[FAST_IPC_MAIN_STACK_SIZE];

/*内核接收消息用*/
static uint8_t cons_msg_buf_main[MSG_BUG_LEN + CONFIG_THREAD_MAP_BUF_LEN * WORD_BYTES];
static umword_t *cons_map_buf = (umword_t *)(cons_msg_buf_main + MSG_BUG_LEN); // 映射buf缓存

static volatile umword_t cons_stack_bitmap;
static uint8_t fake_pthread[256 /*FIXME:*/];

/*栈个数代表并发个数*/
static int stack_array_nr;
static size_t stack_item_size;
static uint8_t *cons_stack;
static uint8_t *cons_msg_buf;
static obj_handler_t *cons_thread_th;

static int fast_ipc_dat_copy(ipc_msg_t *dst_ipc, ipc_msg_t *src_ipc, msg_tag_t tag)
{
    memcpy(dst_ipc->map_buf, src_ipc->map_buf,
           MIN(tag.map_buf_len * WORD_BYTES, IPC_MSG_SIZE));
    memcpy(dst_ipc->msg_buf, src_ipc->msg_buf,
           MIN(tag.msg_buf_len * WORD_BYTES, IPC_MSG_SIZE));
    memcpy(dst_ipc->user, src_ipc->user,
           sizeof(dst_ipc->user));
    return 0;
}
static void update_map_buf(void)
{
    for (int i = 0; i < CONFIG_THREAD_MAP_BUF_LEN; i++) {
        if (cons_map_buf[i] == 0) {
            cons_map_buf[i] = vpage_create_raw3(0, 0, handler_alloc()).raw; /*TODO:申请失败检查*/
        }
    }
}
static msg_tag_t process_ipc(int j, umword_t obj, long tag)
{
    rpc_svr_obj_t *svr_obj;
    ipc_msg_t *msg;
    msg_tag_t ret_tag;
    int ret = 0;

    msg = (ipc_msg_t *)(&cons_msg_buf[j * MSG_BUG_LEN]);
    ret_tag = msg_tag_init4(0, 0, 0, -EIO);
    svr_obj = (rpc_svr_obj_t *)obj;
    if (svr_obj == NULL) {
        ret_tag = msg_tag_init4(0, 0, 0, -EACCES);
        goto end;
    }
    if (svr_obj == (void *)MAGIC_NS_USERPID) {
        /*获取ns的user id*/
        svr_obj = meta_find_svr_obj(NS_PROT);
    }
    if (svr_obj == NULL) {
        ret_tag = msg_tag_init4(0, 0, 0, -EACCES);
        goto end;
    }
    if (svr_obj->dispatch) {
        ret_tag = svr_obj->dispatch(svr_obj, msg_tag_init(tag), msg);
    }
end:
    return ret_tag;
}
static void update_map_buf_last(void)
{
    for (int i = 0; i < CONFIG_THREAD_MAP_BUF_LEN; i++) {
        vpage_t vpage = vpage_create_raw(cons_map_buf[i]);
        if (handler_is_used(vpage.addr)) {
            if (task_obj_valid(TASK_THIS, vpage.addr, 0).prot == 1) {
                cons_map_buf[i] = vpage_create_raw3(0, 0, handler_alloc()).raw;
            }
        }
    }
}
static void fast_ipc_goto_process(int j, long tag, umword_t obj, umword_t arg1, umword_t arg2)
{
    msg_tag_t ret_tag;
    ipc_msg_t *msg;

    msg =  (void *)(&cons_msg_buf[j * MSG_BUG_LEN]);
    thread_msg_buf_set(-1, msg);
    update_map_buf();
    msg->user[3] = cons_thread_th[j];//设置私有变量，FIXME: 一种更加通用的方法
    task_com_unlock(TASK_THIS);
    ret_tag = process_ipc(j, obj, tag);
    task_com_lock(TASK_THIS);
    update_map_buf_last();
    fast_ipc_dat_copy((void *)cons_msg_buf_main, (void *)(&cons_msg_buf[j * MSG_BUG_LEN]), ret_tag);
    thread_ipc_fast_replay(ret_tag, -1, j);
}
static __attribute__((optimize(0))) void fast_ipc_com_point(msg_tag_t tag, umword_t arg0, umword_t arg1, umword_t arg2)
{
    int i;
    for (i = 0; i < stack_array_nr; i++) {
        if ((cons_stack_bitmap & (1 << i)) == 0) {
            cons_stack_bitmap |= (1 << i);
            break;
        }
    }
    fast_ipc_dat_copy((void *)&cons_msg_buf[i * MSG_BUG_LEN], (void *)cons_msg_buf_main, tag);
    fast_ipc_goto_process(fast_ipc_setsp(i, &cons_stack[(i + 1) * stack_item_size - 8]), tag.raw, arg0, arg1, arg2);
}
int u_fast_ipc_init(uint8_t *stack_array, uint8_t *msg_buf_array, int stack_msgbuf_array_num, size_t stack_size, obj_handler_t *threads_obj)
{
    msg_tag_t tag;

    stack_array_nr = stack_msgbuf_array_num;
    cons_stack = stack_array;
    stack_item_size = stack_size;
    cons_msg_buf = msg_buf_array;
    cons_thread_th = threads_obj;
    ipc_msg_t *msg = (void *)cons_msg_buf_main;
    for (int i = 0; i < CONFIG_THREAD_MAP_BUF_LEN; i++) {
        cons_map_buf[i] = vpage_create_raw3(0, 0, handler_alloc()).raw;
        msg->map_buf[i] = cons_map_buf[i];
    }
    msg->user[0] = (umword_t)((char *)fake_pthread + sizeof(fake_pthread));

    tag = task_set_com_point(TASK_THIS, &fast_ipc_com_point, (addr_t)com_stack,
                             sizeof(com_stack), (void *)(&cons_stack_bitmap),
                             stack_msgbuf_array_num, cons_msg_buf_main);
    if (msg_tag_get_val(tag) < 0) {
        return msg_tag_get_val(tag);
    }
    return msg_tag_get_val(tag);
}
