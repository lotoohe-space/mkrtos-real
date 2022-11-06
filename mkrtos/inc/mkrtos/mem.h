#ifndef _MEM_H__
#define _MEM_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "type.h"

/**
* @breif ����ϵͳʹ�õ��ڴ��
*/
#define OS_USE_MEM_AREA_INX 	1

#ifndef NOT_USE_EX_MEM
#define OS_USE_MEM_AREA_INX1 	1
#endif
/**
* @breif ϵͳ������ڴ�������
*/
#ifndef NOT_USE_EX_MEM
#define MEM_AREA_NUM 2
#endif
#ifdef NOT_USE_EX_MEM
#define MEM_AREA_NUM 1
#endif

/**
* @breif �ڴ��С
*/
#define MALLOC_MEM0_SIZE (40*1024)
/**
* @breif �ڴ���С
*/
#define MALLOC_BLOCK0_SIZE 64
/**
* @breif �ڴ���С
*/
#define MALLOC_MANAGER0_TABLE (MALLOC_MEM0_SIZE/MALLOC_BLOCK0_SIZE)

#ifndef NOT_USE_EX_MEM
#define MALLOC_EX_MEM_SIZE (960*1024)
#define MALLOC_EX_BLOCK_SIZE (64)
#define MALLOC_EX_MANAGER_TABLE (MALLOC_EX_MEM_SIZE/MALLOC_EX_BLOCK_SIZE)
#endif
/**
 * �ڴ������������
 */
struct mem_struct {
    struct mem_struct* next;
    void *mem_start;
    struct inode * inode;//���ӳ�䵽���ļ����������Ч���������ΪNULL
    int32_t length;
    int32_t ofst;
};
/**
* @breif �ڴ�����
*/
typedef uint8_t mem_type;
/**
* @breif �ڴ���������
*/
typedef uint32_t mem_tb_type;


/**
* @breif ��ʼ���ڴ����
*/
void init_mem(void) ;
/**
* @breif �����ڴ�
* @param inxMem ����һ���ڴ��������
* @param size ��Ҫ������ڴ��С
*/
void* _malloc(uint16_t inxMem, uint32_t size) ;
/**
* @brief �ͷ�������ڴ�
* @param inxMem ����һ���ڴ����ͷ�
* @param mem_addr �ͷŵ��ڴ��׵�ַ
*/
int32_t _free(uint16_t inxMem, void* mem_addr) ;
/**
* @breif ��ȡʣ����ڴ�
* @param inxMem �ڴ������
* @return ����ʣ������ֽ�
*/
uint32_t get_free_memory(uint8_t mem_no);
uint32_t get_total_memory(uint8_t mem_no);
/**
* @breif �����ڴ�
* @param size ����Ĵ�С
* @return �������뵽���ڴ棬ʧ���򷵻�NULL
*/
void* os_malloc(uint32_t size) ;
void* os_realloc(void *mem,uint32_t size);
#ifndef NOT_USE_EX_MEM
void *os_realloc_ex(void* mem,uint32_t size);
void* os_malloc_ex(uint32_t size) ;
void os_free_ex(void* mem) ;
#endif
/**
* @breif �ͷ�������ڴ�
* @param mem �ͷŵ��ڴ���׵�ַ
*/
void os_free(void* mem) ;
void mem_clear(void);
#ifdef __cplusplus
}
#endif

#endif
