#include "u_types.h"
#include "uart4.h"
#include "u_sys.h"
#include "delay.h"
#include "u_str.h"
#include "u_queue.h"
#include "u_log.h"
#include "u_env.h"
#include "u_sleep.h"
#include "e180-zg120.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief 包类型定义
 *
 */
#define TYPE_CFG 0x00
#define TYPE_ZDO_REQ 0x01
#define TYPE_ZCL_SEND 0x02
#define TYPE_NOTIFY 0x80
#define TYPE_ZDO_RSP 0x81
#define TYPE_ZCL_IND 0x82
#define TYPE_SEND_CNF 0x8F

/**
 * @brief 基础的包数据定义
 *
 */
typedef struct base_pack
{
    uint8_t head;
    uint8_t len;
    uint8_t cmd_type;
    uint8_t cmd_code;
    uint8_t data[];
} base_pack_t;

#pragma pack(1)
/**
 * @brief 网络状态变更数据包
 *
 */
typedef struct notify_0x01
{
    uint8_t net_status;
    uint8_t mac[8];
    uint8_t channel;
    uint16_t panid;
    uint16_t short_addr;
    uint8_t ext_panid[8];
    uint8_t net_key[16];
} notify_0x01_t;

#pragma pack(1)
/**
 * @brief 模组复位发送数据包
 *
 */
typedef struct local_send_0x04
{
    uint8_t reset_mode;
    uint16_t panid;
    uint8_t channel;
} local_send_0x04_t;
#pragma pack(1)
/**
 * @brief 读取入网节点的发送包
 *
 */
typedef struct local_send_0x22
{
    uint16_t addr_inx;
    uint8_t search_mode;
} local_send_0x22_t;
#pragma pack(1)
/**
 * @brief 读取入网节点的反馈包
 *
 */
typedef struct local_read_0x22
{
    uint8_t status;
    uint16_t addr_inx;
    uint16_t short_addr;
    uint8_t mac[8];
    uint8_t flags;
} local_read_0x22_t;

#pragma pack(1)
typedef struct zcl_request_pack // 输入命令“ZCL 请求”格式
{
    uint8_t send_mode;    // 0x40– APS 加密，0x80-强行发送（不路由不转发）
    uint16_t short_addr;  // 发送目标短地址，0xFFFC~0xFFFF 为广播（0xFFFE 为无效地址）
    uint8_t port;         // 发送目标的端口，填入 0xFF 且短地址不为广播时，则采用组播发送
    uint8_t frame_seq;    // 上位机产生帧序号，如果收到 ZCL 帧的帧序号和短地址，端口与发送相等，则该消 息为目标设备的回复消息。
    uint8_t cmd_dir;      // 参照 ZCL 构架，0 - C2S，1 – S2C
    uint16_t clu_id;      // 发送消息的簇 ID，小端模式。
    uint16_t vendor_code; // 发送消息的厂商码，目标设备需要支持厂商码才有效，默认填 0x0000。
    // 0 –使用 Default Response 作应答； 1–使用 APS Ack 作应答；
    // 2–不作任何应答即同时关闭 Default Response 和 APS Ack，适用于高速传输且对数据传
    uint8_t reply_mode;
} zcl_request_pack_t;

#pragma pack(1)
typedef struct zcl_send_ctrl_cmd_0x0f // 发送控制命令（命令码 0x0F）
{
    zcl_request_pack_t base_pack;
    uint8_t cmd_id;
    uint8_t data[128]; // 可变长
} zcl_send_ctrl_cmd_0x0f_t;

/**
 * @brief 本地用于存储节点信息的包
 *
 */

typedef struct node_info
{
    uint8_t mac[8];
    uint16_t short_addr;
    uint16_t parent_addr;
    int8_t net_mode;
    int8_t used;
} node_info_t;

#define NODE_NR 80
typedef struct zigbee_info
{
    node_info_t node_list[NODE_NR];
    notify_0x01_t dev_status;
} zigbee_info_t;

static zigbee_info_t zigbee;

static node_info_t *alloc_node(uint8_t mac[8])
{
    int empty = -1;
    for (int i = 0; i < NODE_NR; i++)
    {
        if (zigbee.node_list[i].used == 1)
        {
            if (memcmp(zigbee.node_list[i].mac, mac, 8) == 0)
            {
                return &zigbee.node_list[i];
            }
        }
        else if (zigbee.node_list[i].used == 0)
        {
            empty = i;
        }
    }
    if (empty >= 0)
    {
        zigbee.node_list[empty].used = 1;
        return &zigbee.node_list[empty];
    }
    return NULL;
}
static void free_node(node_info_t *node)
{
    node->used = 0;
}
static node_info_t *find_node_by_mac(uint8_t mac[8])
{
    for (int i = 0; i < NODE_NR; i++)
    {
        if (zigbee.node_list[i].used == 0)
        {
            continue;
        }
        if (memcmp(mac, zigbee.node_list[i].mac, 8) == 0)
        {
            return &zigbee.node_list[i];
        }
    }
    return NULL;
}

static void cmd_notify_parse(uint8_t *data)
{
    switch (data[0])
    {
    case 0x00:
    {
        printf("==========================================\n");

        printf("reset mode is 0x%x.\n", data[1]);
        printf("software version is 0x%x.\n", data[2]);
        printf("mac :\n");
        print_hex(data + 3, 8);
        printf("==========================================\n");
    }
    break;
    case 0x01:
    {
        printf("==========================================\n");
        notify_0x01_t *pack = (notify_0x01_t *)(data + 1);
        printf("net stat:0x%x\n", pack->net_status);
        printf("mac :\n");
        print_hex(pack->mac, 8);
        printf("channel: 0x%x\n", pack->channel);
        printf("panid: 0x%x\n", (pack->panid));
        printf("short addr: 0x%x\n", (pack->short_addr));
        printf("ext panid :\n");
        print_hex(pack->ext_panid, 8);
        printf("net key :\n");
        print_hex(pack->net_key, 16);
        zigbee.dev_status = *pack;
        printf("==========================================\n");
    }
    break;
    case 0x03:
    {
        printf("==========================================\n");
        node_info_t *node = alloc_node(data + 1);

        if (node == NULL)
        {
            ulog_write_str(u_get_global_env()->log_hd, "No nodes are available.\n");
            return;
        }
        memcpy(node->mac, data + 1, 8);
        node->short_addr = ((data + 1 + 8)[1] << 8) | (data + 1 + 8)[0];
        node->parent_addr = ((data + 1 + 8 + 2)[1] << 8) | (data + 1 + 8 + 2)[0];
        node->net_mode = (data + 1 + 8 + 2 + 2)[0];

        printf("new dev:\n");
        printf("mac :\n");
        print_hex(node->mac, 8);
        printf("short addr:0x%x, parent addr:0x%x, net mode:0x%x\n",
               node->short_addr, node->parent_addr, node->net_mode);
        printf("==========================================\n");
    }
    break;
    case 0x04:
    {
        printf("==========================================\n");
        uint8_t *n_mac = data + 1;

        node_info_t *find_node = find_node_by_mac(n_mac);

        if (find_node == NULL)
        {
            break;
        }
        find_node->short_addr = ((data + 1 + 8)[1] << 8) | (data + 1 + 8)[0];
        printf("update short addr:\n");
        printf("mac :\n");
        print_hex(n_mac, 8);
        printf("new short addr is 0x%x.\n", find_node->short_addr);
        printf("==========================================\n");
    }
    break;
    case 0x06:
    {
        printf("==========================================\n");
        uint8_t *n_mac = data + 1;

        node_info_t *find_node = find_node_by_mac(n_mac);

        if (find_node == NULL)
        {
            break;
        }
        free_node(find_node);
        printf("==========================================\n");
    }
    break;
    }
}
#pragma pack(1)
/**
 * @brief 查询模组当前状态（命令码 0x00）
 *
 */
typedef struct local_read_0x00
{
    uint8_t net_status;
    uint8_t dev_type;
    uint8_t mac[8];
    uint8_t channel;
    uint16_t panid;
    uint16_t short_addr;
    uint8_t ext_panid[8];
    uint8_t net_key[16];
} local_read_0x00_t;
static void cmd_local_parse(uint8_t *data)
{

    switch (data[0])
    {
    case 0x00:
    {
        printf("==========================================\n");
        local_read_0x00_t *pack = (local_read_0x00_t *)(data + 1);

        printf("net stat:0x%x\n", pack->net_status);
        printf("dev type:0x%x\n", pack->dev_type);
        printf("mac :\n");
        print_hex(pack->mac, 8);
        printf("channel: 0x%x\n", pack->channel);
        printf("panid: 0x%x\n", (pack->panid));
        printf("short addr: 0x%x\n", (pack->short_addr));
        printf("ext panid :\n");
        print_hex(pack->ext_panid, 8);
        printf("net key :\n");
        print_hex(pack->net_key, 16);
        printf("==========================================\n");
    }
    break;
    case 0x22:
    {
        local_read_0x22_t *pack = (local_read_0x22_t *)(data + 1);

        if (pack->status == 0 && pack->short_addr != 0xffff)
        {
            printf("==========================================\n");
            node_info_t *node = alloc_node(pack->mac);

            if (node == NULL)
            {
                ulog_write_str(u_get_global_env()->log_hd, "No nodes are available.\n");
                return;
            }
            memcpy(node->mac, pack->mac, 8);
            node->short_addr = (pack->short_addr);

            printf("query new dev:\n");
            printf("mac :\n");
            print_hex(node->mac, 8);
            printf("short addr: 0x%x\n", node->short_addr);
            printf("==========================================\n");
        }
    }
    break;
    }
}
static void cmd_zcl_send_parse(uint8_t *data, int len)
{

    switch (data[0])
    {
    case 0x0F:
    {
        printf("==========================================\n");

        zcl_request_pack_t *pack = (zcl_request_pack_t *)(data + 1);

        int data_len = len - 5 - 1 - sizeof(zcl_request_pack_t);

        printf("recv data:\n");
        print_hex(data + 1 + sizeof(zcl_request_pack_t) + 1, data_len);
        printf("==========================================\n");
    }
    break;
    }
}
static void cmd_parse(uint8_t *data, int len)
{
    switch (data[2])
    {
    case TYPE_NOTIFY:
        cmd_notify_parse(data + 3);
        break;
    case TYPE_CFG:
        cmd_local_parse(data + 3);
        break;
    case TYPE_ZCL_IND:
        cmd_zcl_send_parse(data + 3, len);
        break;
    }
}

void zigbee_send_bytes(uint8_t *bytes, int len)
{
    uart4_send_bytes(bytes, len);
}
/**
 * @brief 计算校验和
 *
 * @param buffer
 * @param len
 * @return uint8_t
 */
static uint8_t zigbee_cal_check_sum(uint8_t *buffer, int len)
{
    uint8_t xor = 0;

    for (int j = 0; j < len; j++)
    {
        xor ^= buffer[j];
    }
    return xor;
}
/**
 * @brief 检查校验和
 *
 * @param buffer
 * @param len
 * @return bool_t
 */
bool_t zigbee_check_sum(uint8_t *buffer, int len)
{
    uint8_t xor = 0;

    for (int j = 2; j < len - 1; j++)
    {
        xor ^= buffer[j];
    }
    return buffer[len - 1] == xor;
}
/**
 * @brief 生成一个数据包
 *
 * @param buffer
 * @param cmd_type
 * @param cmd_code
 * @param data
 * @param data_len
 * @return int
 */
int zigbee_gen_pack(
    uint8_t *buffer, uint8_t cmd_type, uint8_t cmd_code,
    uint8_t *data,
    uint8_t data_len)
{
    int i = 0;
    buffer[i++] = 0x55;
    buffer[i++] = 3 + data_len;
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

    for (int j = 2; j < i; j++)
    {
        xor ^= buffer[j];
    }
    buffer[i++] = xor;
    return i;
}
/**
 * @brief 解析一个数据包
 *
 * @param que
 * @param o_data
 * @return int
 */
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
                    cmd_parse(o_data, pack_len);
                    return pack_len;
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
/**
 * @brief 等待获取数据包
 *
 */
static void wait_pack(int wait_timeout)
{
    umword_t last_tick = sys_read_tick();
    queue_t *q = uart4_queue_get();
    int last_len = q_queue_len(q);
    while ((sys_read_tick() - last_tick) < wait_timeout)
    {
        if (last_len != q_queue_len(q))
        {
            // 长度发生了变化，则更新时间
            last_tick = sys_read_tick();
            last_len = q_queue_len(q);
        }
        // u_sleep_ms(1);
    }
}
/**
 * @brief 读取入网节点的信息
 *
 * @param inx 读取的编号
 */
void local_read_net_node_0x22(uint16_t inx)
{
    uint8_t data_cache[64] = {0};
    int len;
    local_send_0x22_t cmd;
    cmd.addr_inx = inx;
    cmd.search_mode = 0;

    len = zigbee_gen_pack(data_cache, TYPE_CFG, 0x22, (uint8_t *)(&cmd), sizeof(cmd));
    zigbee_send_bytes(data_cache, len);

    wait_pack(50);
    queue_t *q = uart4_queue_get();
    int ret;
    while ((ret = zigbee_pack_get(q, data_cache)) > 0)
    {
        // print_hex(data_cache, ret);
    }
}
/**
 * @brief 读取所有入网节点的信息
 *
 */
void local_read_net_node_0x22_all(void)
{
    for (int i = 0; i < NODE_NR; i++)
    {
        local_read_net_node_0x22(i);
    }
}
/**
 * @brief 设置节点的panid，相同的panid代表同一个网络中。
 *
 * @param panid
 * @return int
 */
int local_set_panid(uint16_t panid)
{
    int ret_op = -1;
    uint8_t data_cache[64] = {0};
    int len;

    len = zigbee_gen_pack(data_cache, TYPE_CFG, 0x08, (uint8_t *)(&panid), sizeof(panid));
    zigbee_send_bytes(data_cache, len);
    wait_pack(50);
    queue_t *q = uart4_queue_get();
    int ret;
    while ((ret = zigbee_pack_get(q, data_cache)) > 0)
    {
        base_pack_t *pack = (base_pack_t *)data_cache;

        if (pack->cmd_type == TYPE_CFG && pack->cmd_code == 0x08)
        {
            if (pack->data[0] == 0)
            {
                ret_op = 0;
            }
        }
        print_hex(data_cache, ret);
    }
    return ret_op;
}
/**
 * @brief 请求模块状态
 *
 */
void mod_query_status(void)
{
    uint8_t data_cache[64];
    int len;

    len = zigbee_gen_pack(data_cache, TYPE_CFG, 0x00, NULL, 0);
    zigbee_send_bytes(data_cache, len);

    wait_pack(50);
    queue_t *q = uart4_queue_get();
    int ret;
    while ((ret = zigbee_pack_get(q, data_cache)) > 0)
    {
        print_q_hex(q);
    }
}
/**
 * @brief  模块自启动
 *
 * @param auto_start
 * @return int
 */
int mod_start(uint8_t auto_start)
{
    uint8_t data_cache[64];
    int len;
    int ret_op = -1;

    auto_start = !!auto_start;
    len = zigbee_gen_pack(data_cache, TYPE_CFG, 0x01, &auto_start, 1);

    zigbee_send_bytes(data_cache, len);

    wait_pack(50);
    queue_t *q = uart4_queue_get();

    int ret;
    while ((ret = zigbee_pack_get(q, data_cache)) > 0)
    {
        base_pack_t *pack = (base_pack_t *)data_cache;

        if (pack->cmd_type == TYPE_CFG && pack->cmd_code == 0x01)
        {
            if (pack->data[0] == 0)
            {
                ret_op = 0;
            }
        }
        print_q_hex(q);
    }
    return ret_op;
}

/**
 * @brief 模块复位
 *
 * @param reset_mode 0 - 模组复位； 1- 模组退网； 2 – 模组恢复出厂
 * @param panid
 * @param channel
 */
int mod_reset(int reset_mode, uint16_t panid, uint8_t channel)
{
    uint8_t data_cache[64] = {0};
    int len;
    int ret_op = -1;
    local_send_0x04_t pack = {
        .channel = channel,
        .panid = panid,
        .reset_mode = reset_mode,
    };

    len = zigbee_gen_pack(data_cache, TYPE_CFG, 0x04, (uint8_t *)(&pack), sizeof(pack));
    zigbee_send_bytes(data_cache, len);

    wait_pack(100);
    queue_t *q = uart4_queue_get();
    int ret;
    while ((ret = zigbee_pack_get(q, data_cache)) > 0)
    {
        base_pack_t *pack = (base_pack_t *)data_cache;

        if (pack->cmd_type == TYPE_CFG && pack->cmd_code == 0x04)
        {
            if (pack->data[0] == 0)
            {
                ret_op = 0;
            }
        }
        print_hex(data_cache, ret);
    }
    return ret_op;
}
/**
 * @brief 设置节点类型
 *
 * @param p_type
 * @return int
 */
int mod_set_node_type(enum point_type p_type)
{
    int len;
    int ret_op = -1;
    uint8_t data_cache[64] = {0};

    len = zigbee_gen_pack(data_cache, TYPE_CFG, 0x05, &p_type, 1);
    zigbee_send_bytes(data_cache, len);

    wait_pack(100);
    queue_t *q = uart4_queue_get();

    int ret;
    while ((ret = zigbee_pack_get(q, data_cache)) > 0)
    {
        base_pack_t *pack = (base_pack_t *)data_cache;

        if (pack->cmd_type == TYPE_CFG && pack->cmd_code == 0x05)
        {
            if (pack->data[0] == 0)
            {
                ret_op = 0;
            }
        }
        print_hex(data_cache, ret);
    }
    return ret_op;
}
/**
 * @brief 模组开始配网
 *
 * @param mode
 * @return int
 */
int mod_start_cfg_net(uint8_t mode)
{
    int len;
    int ret_op = -1;
    uint8_t data_cache[64] = {0};

    len = zigbee_gen_pack(data_cache, TYPE_CFG, 0x02, &mode, 1);
    zigbee_send_bytes(data_cache, len);

    wait_pack(100);
    queue_t *q = uart4_queue_get();

    int ret;
    while ((ret = zigbee_pack_get(q, data_cache)) > 0)
    {
        base_pack_t *pack = (base_pack_t *)data_cache;

        if (pack->cmd_type == TYPE_CFG && pack->cmd_code == 0x02)
        {
            if (pack->data[0] == 0)
            {
                ret_op = 0;
            }
        }
        print_hex(data_cache, ret);
    }
    return ret_op;
}

int mod_send_data(uint16_t short_addr, uint8_t port, uint8_t dir,
                  uint8_t cmd_id, uint8_t *data, uint8_t len)
{
    static uint8_t seq = 0;
    int send_len;
    int ret_op = -1;
    static uint8_t data_cache[256];
    zcl_send_ctrl_cmd_0x0f_t send_pack = {
        .base_pack.clu_id = EBYTE_CLUSTER,
        .base_pack.cmd_dir = dir,
        .base_pack.frame_seq = seq++,
        .base_pack.port = port,
        .base_pack.reply_mode = 0,
        .base_pack.send_mode = 0,
        .base_pack.short_addr = short_addr,
        .base_pack.vendor_code = 0x2000,
        .cmd_id = cmd_id,
    };
    int cp_len = MIN(len, sizeof(send_pack.data));
    memcpy(send_pack.data, data, cp_len);

    send_len = zigbee_gen_pack(data_cache, TYPE_ZCL_SEND, 0x0F, (uint8_t *)(&send_pack), sizeof(zcl_request_pack_t) + 1 + cp_len);
    zigbee_send_bytes(data_cache, send_len);
    print_hex(data_cache, send_len);
    wait_pack(100);
    queue_t *q = uart4_queue_get();

    int ret;
    while ((ret = zigbee_pack_get(q, data_cache)) > 0)
    {
        base_pack_t *pack = (base_pack_t *)data_cache;

        if (pack->cmd_type == TYPE_ZCL_SEND && pack->cmd_code == 0x0F)
        {
            if (pack->data[0] == 0 && pack->data[1] == send_pack.base_pack.frame_seq)
            {
                ret_op = 0;
            }
        }
        print_hex(data_cache, ret);
    }

    return ret_op;
}

void e180_loop(void)
{
    {
        queue_t *q = uart4_queue_get();

        if (q_queue_len(q) > 0)
        {
            wait_pack(50);
            if (q_queue_len(q) > 0)
            {
                uint8_t data_cache_read[128];
                int ret;
                while ((ret = zigbee_pack_get(q, data_cache_read)) > 0)
                {
                    print_hex(data_cache_read, ret);
                }
            }
            // uart4_recv_flags = 0;
        }
    }
}