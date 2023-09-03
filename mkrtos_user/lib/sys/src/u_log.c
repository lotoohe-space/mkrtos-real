

#include "u_log.h"
#include "u_prot.h"
#include "u_types.h"
#include "u_util.h"
#include <string.h>
enum log_op
{
    WRITE_DATA,
    READ_DATA,
    SET_FLAGS
};
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
            syscall(syscall_prot_create(WRITE_DATA, LOG_PROT, obj_inx),
                    msg_tag_init3(0, ipc_type_create_3(MSG_NONE_TYPE, i, 0).raw, LOG_PROT).raw,
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
    }
}

void ulog_write_str(obj_handler_t obj_inx, const char *str)
{
    size_t i;
    for (i = 0; str[i]; i++)
        ;
    ulog_write_bytes(obj_inx, (uint8_t *)str, i + 1);
}
