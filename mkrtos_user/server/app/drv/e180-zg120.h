#pragma once
#include "u_types.h"

enum point_type
{
    COD_TYPE = 0,
    ROUTE_TYPE,
    END_POINT_TYPE,
    SLEEP_END_POINT_TYPE,
};

enum cluster
{
    EBYTE_CLUSTER = 0xFC08,
};

int mod_start_cfg_net(uint8_t mode);
int mod_send_data(uint16_t short_addr, uint8_t port, uint8_t dir,
                  uint8_t cmd_id, uint8_t *data, uint8_t len);

int mod_set_node_type(enum point_type p_type);
int mod_reset(int reset_mode, uint16_t panid, uint8_t channel);
void mod_query_status(void);
int mod_start(uint8_t auto_start);
void local_read_net_node_0x22_all(void);
int local_set_panid(uint16_t panid);
void e180_loop(void);
