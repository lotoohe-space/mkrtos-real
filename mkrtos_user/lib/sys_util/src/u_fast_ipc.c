
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
#include <stdio.h>
#define MAGIC_PM_USERPID 0xbabababa

int fast_ipc_setsp(int i, void *stack);

#define FAST_IPC_MAIN_STACK_SIZE 512
static ATTR_ALIGN(8) uint8_t com_stack[FAST_IPC_MAIN_STACK_SIZE];

/*内核接收消息用*/
// static uint8_t cons_msg_buf_main[MSG_BUG_LEN];
static umword_t (*cons_map_buf)[CONFIG_THREAD_MAP_BUF_LEN]; // 映射buf缓存

static volatile umword_t cons_stack_bitmap;
static uint8_t fake_pthread[4][256 /*FIXME:*/];

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
static void update_map_buf(int fipc_channel)
{
    umword_t *_cons_map_buf = cons_map_buf[fipc_channel];

    for (int i = 0; i < CONFIG_THREAD_MAP_BUF_LEN; i++)
    {
        if (_cons_map_buf[i] == 0)
        {
            // 在reply的时候被映射使用了
            _cons_map_buf[i] = vpage_create_raw3(0, 0, handler_alloc()).raw; /*TODO:申请失败检查*/
        }
        else if (handler_is_used(vpage_create_raw(_cons_map_buf[i]).addr))
        {
            if (u_task_obj_valid(TASK_THIS, vpage_create_raw(_cons_map_buf[i]).addr, NULL).prot == 1)
            {
                // client映射obj过来的时候占用了，重新分配一个新的
                _cons_map_buf[i] = vpage_create_raw3(0, 0, handler_alloc()).raw;
            }
        }
        else
        {
            // fd没有申请，则申请一个新的
            _cons_map_buf[i] = vpage_create_raw3(0, 0, handler_alloc()).raw;
        }
    }
}
static msg_tag_t process_ipc(int j, umword_t obj, long tag)
{
    rpc_svr_obj_t *svr_obj;
    ipc_msg_t *msg;
    msg_tag_t ret_tag;

    msg = (ipc_msg_t *)(&cons_msg_buf[j * MSG_BUG_LEN]);
    ret_tag = msg_tag_init4(0, 0, 0, -EIO);
    svr_obj = (rpc_svr_obj_t *)obj;
    if (svr_obj == NULL)
    {
        ret_tag = msg_tag_init4(0, 0, 0, -EACCES);
        goto end;
    }
    if (svr_obj == (void *)MAGIC_PM_USERPID)
    {
        /*获取ns的user id*/
        svr_obj = meta_find_svr_obj(PM_PROT);
    }
    if (svr_obj == NULL)
    {
        ret_tag = msg_tag_init4(0, 0, 0, -EACCES);
        goto end;
    }
    if (svr_obj->dispatch)
    {
        ret_tag = svr_obj->dispatch(svr_obj, msg_tag_init(tag), msg);
    }
end:
    return ret_tag;
}
// static void update_map_buf_last(void)
// {
//     // 检查下缓存里面的fd是不是已经被使用了，如果被使用了就需要更新新的。
//     for (int i = 0; i < CONFIG_THREAD_MAP_BUF_LEN; i++)
//     {
//         vpage_t vpage = vpage_create_raw(cons_map_buf[i]);
//         if (handler_is_used(vpage.addr))
//         {
//             if (u_task_obj_valid(TASK_THIS, vpage.addr, NULL).prot == 1)
//             {
//                 cons_map_buf[i] = vpage_create_raw3(0, 0, handler_alloc()).raw;
//             }
//         }
//         else
//         {
//             cons_map_buf[i] = vpage_create_raw3(0, 0, handler_alloc()).raw;
//         }
//     }
// }
static void fast_ipc_goto_process(int j, long tag, umword_t obj, umword_t fipc_channel, umword_t arg2)
{
    msg_tag_t ret_tag;
    ipc_msg_t *msg;

    msg = (void *)(&cons_msg_buf[fipc_channel * MSG_BUG_LEN]);
    u_thread_msg_buf_set(-1, msg);
    update_map_buf(fipc_channel);
    u_task_com_unlock(TASK_THIS);
    ret_tag = process_ipc(j, obj, tag);
    u_task_com_lock(TASK_THIS);
    u_thread_ipc_fast_replay(ret_tag, -1, fipc_channel);
}
static __attribute__((optimize(0))) void fast_ipc_com_point(msg_tag_t tag, umword_t obj_ptr, umword_t fipc_channel, umword_t arg2)
{
    fast_ipc_goto_process(fast_ipc_setsp(fipc_channel, &cons_stack[(fipc_channel + 1) * stack_item_size - 8]),
                          tag.raw, obj_ptr, fipc_channel, arg2);
}
static bool_t is_init;
bool_t u_fast_ipc_is_init(void)
{
    return is_init;
}

int u_fast_ipc_init(
    uint8_t *stack_array, uint8_t *msg_buf_array, int stack_msgbuf_array_num,
    size_t stack_size,
    obj_handler_t *threads_obj,
    umword_t (*cons_map_buf_in)[CONFIG_THREAD_MAP_BUF_LEN])
{
    msg_tag_t tag;

    stack_array_nr = stack_msgbuf_array_num;
    cons_stack = stack_array;
    stack_item_size = stack_size;
    cons_thread_th = threads_obj;
    cons_msg_buf = msg_buf_array;
    cons_map_buf = cons_map_buf_in;

    for (int j = 0; j < stack_msgbuf_array_num; j++)
    {
        ipc_msg_t *msg = (void *)(&cons_msg_buf[j * MSG_BUG_LEN]);
        for (int i = 0; i < CONFIG_THREAD_MAP_BUF_LEN; i++)
        {
            // 预先分配用于在通信时映射的fd
            cons_map_buf[j][i] = vpage_create_raw3(0, 0, handler_alloc()).raw;
            msg->map_buf[i] = cons_map_buf[j][i];
        }

        msg->user[0] = (umword_t)((char *)(fake_pthread[j]) + sizeof(fake_pthread));
        msg->user[3] = threads_obj[j];
    }

#if 1
    for (int i = 0; i < stack_msgbuf_array_num; i++)
    {
        printf("fastipc: stack 0x%x %x\n", stack_array + stack_size * i, stack_size);
    }
#endif
    fast_ipc_info_t fipc_info =
        {
            .com_point_func = fast_ipc_com_point,
            .stack = (addr_t)com_stack,
            .stack_size = stack_size,
            .bitmap = (void *)(&cons_stack_bitmap),
            .bitmap_len = stack_array_nr,
            .msg_buf = (void *)cons_msg_buf,
            .map_buf = cons_map_buf,
        };
    tag = u_task_set_com_point(TASK_THIS, &fipc_info);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    is_init = TRUE;
    return msg_tag_get_val(tag);
}
