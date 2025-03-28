#include "cons.h"
#define ULOG_RW_MAX_BYTES (WORD_BYTES * 5)
#define WRITE_DATA 0
typedef union syscall_prot
{
    umword_t raw;
    struct
    {
        umword_t op : 6;                     //!< 操作的op
        umword_t prot : 5;                   //!< 通信的类型
        umword_t self : 1;                   //!< 如果obj_inx指定为无效，则采用当前thread
        umword_t obj_inx : (WORD_BITS - 12); //!<
    };
} syscall_prot_t;
#define LOG_PROT 4

static inline syscall_prot_t syscall_prot_create(uint8_t op, uint8_t prot, obj_handler_t obj_inx)
{
    return (syscall_prot_t){
        .op = op,
        .prot = prot,
        .obj_inx = obj_inx,
        .self = 0,
    };
}
typedef union msg_tag
{
    umword_t raw;
    struct
    {
        umword_t flags : 4; // 3bit:代表错误由内核报告
        umword_t msg_buf_len : 7;
        umword_t map_buf_len : 5;
        umword_t prot : WORD_BITS - 16;
    };
} msg_tag_t;

#define msg_tag_init(r) \
    ((msg_tag_t){.raw = (r)})

#define msg_tag_is_knl_err(tag) (!!((tag).flags & MSG_TAG_KNL_ERR)) //!< 内核错误

#define msg_tag_init4(fg, msg_words, buf_words, p) ((msg_tag_t){ \
    .flags = (fg),                                               \
    .msg_buf_len = (msg_words),                                  \
    .map_buf_len = (buf_words),                                  \
    .prot = (p)})
void *__aeabi_memset(void *dest, int c, int n)
{
	unsigned char *s = dest;

    for (; n; n--, s++) *s = c;

    return dest;
}
void u_log_write_bytes(obj_handler_t obj_inx, const uint8_t *data, int len)
{
    uint8_t write_buf[ULOG_RW_MAX_BYTES] = {0};
    int j = 0;
    int i = 0;

    while (1)
    {
        for (i = 0; i < ULOG_RW_MAX_BYTES && j < len; i++, j++)
        {
            write_buf[i] = data[j];
        }
        if (i > 0)
        {
            int *write_word_buf = (int *)write_buf;
            mk_syscall(syscall_prot_create(WRITE_DATA, LOG_PROT, obj_inx).raw,
                       msg_tag_init4(0, ROUND_UP(i, WORD_BYTES), 0, 0).raw,
                       write_word_buf[0],
                       write_word_buf[1],
                       write_word_buf[2],
                       write_word_buf[3],
                       write_word_buf[4]);
        }
        if (j >= len)
        {
            return;
        }
        for (int i = 0; i < ULOG_RW_MAX_BYTES; i++)
        {
            write_buf[i] = 0;
        }
    }
}
