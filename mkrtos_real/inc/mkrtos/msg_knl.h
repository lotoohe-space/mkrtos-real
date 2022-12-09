//
// Created by Administrator on 2022/5/20.
//

#ifndef UNTITLED1_MSG_KNL_H
#define UNTITLED1_MSG_KNL_H


void msg_check(void) ;
int sys_subscribe_msg(const char* msg_knl_name,void (*fun_cb)(void*msg,int len));
int sys_publish_msg(const char* name, unsigned char* data,int len);

#endif //UNTITLED1_MSG_KNL_H
