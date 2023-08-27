

#include "u_log.h"
#include "u_prot.h"
#include "u_types.h"
#include <string.h>

void ulog_write_bytes(obj_handler_t obj_inx, const uint8_t *data, umword_t len)
{
    uint8_t write_buf[ULOG_RW_MAX_BYTES];
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
            syscall(obj_inx, msg_tag_init3(0, i, LOG_PROT).raw,
                    write_word_buf[0],
                    write_word_buf[1],
                    write_word_buf[2],
                    write_word_buf[3],
                    write_word_buf[4]);
        }
        if (j == len)
        {
            return;
        }
    }
}

void ulog_write_str(obj_handler_t obj_inx, const char *str)
{
    ulog_write_bytes(obj_inx, (uint8_t *)str, strlen(str));
}
