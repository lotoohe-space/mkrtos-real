/*
 * @Author: ATShining 1358745329@qq.com
 * @Date: 2023-08-14 09:47:54
 * @LastEditors: ATShining 1358745329@qq.com
 * @LastEditTime: 2023-08-14 13:19:14
 * @FilePath: /mkrtos-real/mkrtos_knl/inc/lib/assert.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "mm_wrap.h"
#include "printk.h"
#define assert(cond)                                       \
    do {                                                   \
        if (!(cond)) {                                     \
            printk("\n%s:%d %s\n", __FILE__, __LINE__, #cond); \
            dumpstack();                                   \
            mm_trace();                                    \
            while (1)                                      \
                ;                                          \
        }                                                  \
    } while (0)
