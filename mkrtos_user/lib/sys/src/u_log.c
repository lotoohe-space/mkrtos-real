

#include "u_log.h"
#include "u_prot.h"
#include "u_types.h"
#include "u_arch.h"
#include "u_util.h"
#include <string.h>
enum log_op
{
    WRITE_DATA,
    READ_DATA,
    SET_FLAGS
};
MK_SYSCALL
static msg_tag_t ulog_read_bytes_raw(obj_handler_t obj_inx, umword_t data[5], int len)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);
    register volatile umword_t r2 asm(ARCH_REG_2);
    register volatile umword_t r3 asm(ARCH_REG_3);
    register volatile umword_t r4 asm(ARCH_REG_4);
    register volatile umword_t r5 asm(ARCH_REG_5);
    mk_syscall(syscall_prot_create(READ_DATA, LOG_PROT, obj_inx).raw,
               msg_tag_init4(0, 0, 0, 0).raw,
               len,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0, ARCH_REG_1, ARCH_REG_2, ARCH_REG_3, ARCH_REG_4, ARCH_REG_5);
    msg_tag_t tag = msg_tag_init(r0);
    {
        ((umword_t *)data)[0] = r1;
        ((umword_t *)data)[1] = r2;
        ((umword_t *)data)[2] = r3;
        ((umword_t *)data)[3] = r4;
        ((umword_t *)data)[4] = r5;
    }

    return tag;
}
MK_SYSCALL
int ulog_read_bytes(obj_handler_t obj_inx, uint8_t *data, umword_t len)
{
    umword_t buffer[5];

    msg_tag_t tag = ulog_read_bytes_raw(obj_inx, buffer, len);

    if (msg_tag_get_val(tag) > 0)
    {
        for (int i = 0; i < msg_tag_get_val(tag); i++)
        {
            data[i] = ((uint8_t *)buffer)[i];
        }
    }
    return msg_tag_get_val(tag);
}
MK_SYSCALL
void ulog_write_bytes(obj_handler_t obj_inx, const uint8_t *data, umword_t len)
{
    uint8_t write_buf[ULOG_RW_MAX_BYTES] = {0};
    umword_t j = 0;
    int i = 0;

    while (1)
    {
        for (i = 0; i < ULOG_RW_MAX_BYTES && j < len; i++, j++)
        {
            write_buf[i] = data[j];
        }
        if (i > 0)
        {
            umword_t *write_word_buf = (umword_t *)write_buf;
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
MK_SYSCALL
void ulog_write_str(obj_handler_t obj_inx, const char *str)
{
    size_t i;
    for (i = 0; str[i]; i++)
        ;
    ulog_write_bytes(obj_inx, (uint8_t *)str, i + 1);
}
