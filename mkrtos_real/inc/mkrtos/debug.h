//
// Created by Administrator on 2022/4/7.
//

#ifndef UNTITLED1_DEBUG_H
#define UNTITLED1_DEBUG_H

#include <mkrtos/sched.h>
#define DEBUG_ENABLE  1
#define LOG_LEVEL     DEBUG_ERR

#define MODULE "BIGGG"
#define ERR    "ERR"
#define WARN   "WARN"
#define INFO   "INFO"

#define DEBUG_COLOR_ERR       "\033[1;31m"				//������ɫ
#define DEBUG_COLOR_WARN      "\033[1;34m"				//������ɫ
#define DEBUG_COLOR_INFO      "\033[1;33m"              //������ɫ
#define DEBUG_COLOR_END       "\033[1;37m"				//��ɫ

enum DEBUG_LEVEL
{
    DEBUG_OFF = 0,
    DEBUG_ERR,
    DEBUG_WARN,
    DEBUG_INFO,
    DEBUG_ALL
};

#define DEBUG(MODULE, LEVEL, FMT, ARGS...) \
do \
{                                          \
	uint32_t t;                                \
    if(DEBUG_ENABLE && (DEBUG_##LEVEL <= LOG_LEVEL)) \
    { \
      kprint(DEBUG_COLOR_##LEVEL); \
      kprint("[%s]:[%s]:[%s]:%d "FMT"\r\n", MODULE, __FILE__, __FUNCTION__, __LINE__, ##ARGS); \
      kprint(DEBUG_COLOR_END); \
    }         \
} while(0)\


#endif //UNTITLED1_DEBUG_H
