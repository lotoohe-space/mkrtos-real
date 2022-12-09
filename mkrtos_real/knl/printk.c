//
// Created by Administrator on 2022/1/19.
//

#include <stdio.h>
#include <stdarg.h>
#include <type.h>
#include <mkrtos/fs.h>
#include <mkrtos/early_console.h>

#include <stm32f2xx.h>
#include <stdio.h>
#include <xprintf.h>


static uint8_t printk_cache[512];

void ktrace(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    xvsprintf(printk_cache,fmt,args);
    va_end(args);
    console_write(printk_cache,mkrtos_strlen(printk_cache));
}
void kprint(const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    xvsprintf(printk_cache,fmt,args);
    va_end(args);
    console_write(printk_cache,mkrtos_strlen(printk_cache));
}

void kfatal(const char* fmt,...){

    va_list args;
    va_start(args, fmt);
    xvsprintf(printk_cache,fmt,args);
    va_end(args);
    console_write(printk_cache,mkrtos_strlen(printk_cache));
    while(1);
}
