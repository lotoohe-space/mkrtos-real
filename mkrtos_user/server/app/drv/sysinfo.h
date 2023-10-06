#pragma once
#include "u_types.h"

#define CTRL_IO_NUM 10

typedef struct
{
    float temp[6];
    int16_t target_val;

    float board_temp;
    float pressure;
    char text_info[8][12];

    int auto_ctrl_tick[CTRL_IO_NUM];

    float noise[3];
    float noise_temp[3];
    uint8_t relay[6];

    uint8_t netID;
    uint8_t devID;
    int init_val;
} uapp_sys_info_t;

extern uapp_sys_info_t sys_info;

void sys_info_lock(void);
void sys_info_unlock(void);

int sys_info_save(void);
int sys_info_read(void);
