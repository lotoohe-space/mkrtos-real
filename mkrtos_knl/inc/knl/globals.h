/*
 * @Author: zhangzheng
 * @Date: 2023-08-14 09:47:54
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-14 13:10:15
 * @FilePath: /mkrtos-real/mkrtos_knl/inc/knl/globals.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "mm.h"
#include "types.h"
#include "kobject.h"

mem_t *mm_get_global(void);
void global_reg_kobj(kobject_t *kobj, int inx);
