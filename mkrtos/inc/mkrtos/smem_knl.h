#ifndef _OS_SHARE_MEM_H__
#define _OS_SHARE_MEM_H__
#include "type.h"

/**
* @breif ����洢������ֲ���Ҫʹ�ô��ڴ�ʱʹ��malloc�������ܽ���
*/
int32_t share_mem_init(void);
/**
* @breif �����ڴ���
*/
uint8_t* share_mem_lock(uint32_t hope_size);
/**
* @breif �����ڴ����
*/
void share_mem_unlock(void);

#endif
