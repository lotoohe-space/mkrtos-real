#include "u_types.h"
#include "uart4.h"
#include "u_sys.h"
#include "delay.h"
#include "u_str.h"
#include "u_queue.h"

#define TYPE_CFG 0x00
#define TYPE_ZDO_REQ 0x01
#define TYPE_ZCL_SEND 0x02
#define TYPE_NOTIFY 0x80
#define TYPE_ZDO_RSP 0x81
#define TYPE_ZCL_IND 0x82
#define TYPE_SEND_CNF 0x8F

void zigbee_send_bytes(uint8_t *bytes, int len)
{
    // print_hex(bytes, len);

    uart4_send_bytes(bytes, len);
}
static uint8_t zigbee_cal_check_sum(uint8_t *buffer, int len)
{
    uint8_t xor = 0;

    for (int j = 0; j < len; j++)
    {
        xor ^= buffer[j];
    }
    return xor;
}
bool_t zigbee_check_sum(uint8_t *buffer, int len)
{
    uint8_t xor = 0;

    for (int j = 2; j < len - 1; j++)
    {
        xor ^= buffer[j];
    }
    return buffer[len - 1] == xor;
}
int zigbee_gen_pack(
    uint8_t *buffer, uint8_t cmd_type, uint8_t cmd_code,
    uint8_t *data,
    uint8_t data_len)
{
    int i = 0;
    buffer[i++] = 0x55;
    buffer[i++] = 5 + data_len;
    buffer[i++] = cmd_type;
    buffer[i++] = cmd_code;
    if (data)
    {
        for (int j = i; j < data_len + i; j++)
        {
            buffer[j] = data[j - i];
        }
        i += data_len;
    }
    uint8_t xor = 0;

    for (int j = 2; j < i - 2; j++)
    {
        xor ^= buffer[j];
    }
    buffer[i++] = xor;
    return i;
}
int zigbee_pack_get(queue_t *que, uint8_t *o_data)
{
    int i = 0;
    int step = 0;
    int len = 0;
    while (1)
    {
        uint8_t data;

        int ret = q_dequeue(que, &data);

        if (ret < 0)
        {
            break;
        }
        switch (step)
        {
        case 0:
            if (data == 0x55)
            {
                o_data[i] = data;
                i++;
                step++;
            }
            else
            {
                q_queue_clear(que);
                return -1;
            }
            break;
        case 1:
            o_data[i] = data;
            len = data;
            i++;
            step++;
            break;
        case 2:
            o_data[i] = data;
            i++;
            if (i >= len + 2)
            {
                int pack_len = len + 2;

                bool_t suc = zigbee_check_sum(o_data, pack_len);
                if (suc)
                {
                    return len + 2;
                }
                else
                {
                    q_queue_clear(que);
                    return -1;
                }
            }
            break;
        }
    }
    return -1;
}

void query_mod_status(void)
{
    uint8_t data_cache[64];
    int len;

    len = zigbee_gen_pack(data_cache, TYPE_CFG, 0x00, NULL, 0);
    zigbee_send_bytes(data_cache, len);

    umword_t last_tick = sys_read_tick();
    queue_t *q = uart4_queue_get();
    int last_len = q_queue_len(q);
    while (sys_read_tick() - last_tick < 50)
    {
        if (last_len != q_queue_len(q))
        { // 长度发生了变化，则更新时间
            last_tick = sys_read_tick();
            last_len = q_queue_len(q);
        }
    }

    print_q_hex(q);
}

void mod_start(void)
{
    uint8_t data_cache[64];
    int len;

    uint8_t data_0 = 0x1;
    len = zigbee_gen_pack(data_cache, TYPE_CFG, 0x01, &data_0, 1);

    zigbee_send_bytes(data_cache, len);

    umword_t last_tick = sys_read_tick();
    queue_t *q = uart4_queue_get();
    int last_len = q_queue_len(q);
    while (sys_read_tick() - last_tick < 50)
    {
        if (last_len != q_queue_len(q))
        { // 长度发生了变化，则更新时间
            last_tick = sys_read_tick();
            last_len = q_queue_len(q);
        }
    }

    print_q_hex(q);
}
void mod_reset(void)
{
    uint8_t data_cache[64] = {0x55, 0x07, 0x00, 0x04, 0x00, 0xFF, 0xFF, 0x00, 0x04};
    int len;

    zigbee_send_bytes(data_cache, 9);

    umword_t last_tick = sys_read_tick();
    queue_t *q = uart4_queue_get();
    int last_len = q_queue_len(q);
    while ((sys_read_tick() - last_tick) < 100)
    {
        if (last_len != q_queue_len(q))
        { // 长度发生了变化，则更新时间
            last_tick = sys_read_tick();
            last_len = q_queue_len(q);
        }
    }
    int ret;
    while ((ret = zigbee_pack_get(q, data_cache)) > 0)
    {
        print_hex(data_cache, ret);
    }
}
void mod_set_cod(void)
{
    uint8_t data_cache[64] = {0x55, 0x04, 0x00, 0x05, 0x00, 0x05};
    zigbee_send_bytes(data_cache, 6);

    umword_t last_tick = sys_read_tick();
    queue_t *q = uart4_queue_get();
    int last_len = q_queue_len(q);
    while (sys_read_tick() - last_tick < 100)
    {
        if (last_len != q_queue_len(q))
        { // 长度发生了变化，则更新时间
            last_tick = sys_read_tick();
            last_len = q_queue_len(q);
        }
    }

    int ret;
    while ((ret = zigbee_pack_get(q, data_cache)) > 0)
    {
        print_hex(data_cache, ret);
    }
}
void mod_cfg_net(void)
{
    uint8_t data_cache[64] = {0x55, 0x03, 0x00, 0x02, 0x02};
    zigbee_send_bytes(data_cache, 5);

    umword_t last_tick = sys_read_tick();
    queue_t *q = uart4_queue_get();
    int last_len = q_queue_len(q);
    while (sys_read_tick() - last_tick < 100)
    {
        if (last_len != q_queue_len(q))
        { // 长度发生了变化，则更新时间
            last_tick = sys_read_tick();
            last_len = q_queue_len(q);
        }
    }

    int ret;
    while ((ret = zigbee_pack_get(q, data_cache)) > 0)
    {
        print_hex(data_cache, ret);
    }
}
void mod_send_data(uint16_t short_addr)
{
    uint8_t data_cache[] = {0x55, 0x19, 0x02, 0x0F, 0x00,
                            short_addr & 0xff, (short_addr >> 8) & 0xff, // short addr
                            0x01, 0x8C, 0x00, 0x08,
                            0xFC, 0x00, 0x20, 0x00, 0x00, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x29};

    uint8_t check_sum = zigbee_cal_check_sum(data_cache + 2, sizeof(data_cache) - 3);
    data_cache[sizeof(data_cache) - 1] = check_sum;
    zigbee_send_bytes(data_cache, sizeof(data_cache));

    umword_t last_tick = sys_read_tick();
    queue_t *q = uart4_queue_get();
    int last_len = q_queue_len(q);
    while (sys_read_tick() - last_tick < 100)
    {
        if (last_len != q_queue_len(q))
        { // 长度发生了变化，则更新时间
            last_tick = sys_read_tick();
            last_len = q_queue_len(q);
        }
    }

    uint8_t data_cache_read[64];
    int ret;
    while ((ret = zigbee_pack_get(q, data_cache_read)) > 0)
    {
        print_hex(data_cache_read, ret);
    }
}

void e180_loop(void)
{
    {
        umword_t last_tick = sys_read_tick();
        queue_t *q = uart4_queue_get();
        int last_len = q_queue_len(q);
        while (sys_read_tick() - last_tick < 100)
        {
            if (last_len != q_queue_len(q))
            { // 长度发生了变化，则更新时间
                last_tick = sys_read_tick();
                last_len = q_queue_len(q);
            }
        }
        if (q_queue_len(q) > 0)
        {
            uint8_t data_cache_read[64];
            int ret;
            while ((ret = zigbee_pack_get(q, data_cache_read)) > 0)
            {
                print_hex(data_cache_read, ret);
            }
        }
        uart4_recv_flags = 0;
    }
}