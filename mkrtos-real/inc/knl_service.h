/*
 * knl_service.h
 *
 *  Created on: 2022Äê8ÔÂ6ÈÕ
 *      Author: Administrator
 */

#ifndef INC_KNL_SERVICE_H_
#define INC_KNL_SERVICE_H_

#include <type.h>

void *malloc(uint32_t size);
void free(void* mem);
void *malloc_align(uint32_t size,uint32_t align);
void free_align(void *mem);
void *user_malloc(uint32_t size,const char* name);
void user_free(void *mem);
void knl_mem_trace(void);
struct mem_heap *knl_mem_get_free(int32_t hope_size, uint32_t *ret_addr);

//
void mkrtos_memcpy(void *dst,void *src,int len);
char * mkrtos_strcpy (register char *s1, register const char *s2);
void *mkrtos_memccpy(void *dst, const void *src, int c, uint32_t count);
char *mkrtos_strncpy(char *dest, const char *src, uint32_t n) ;
int mkrtos_strcmp(const char *s1, const char *s2);
size_t mkrtos_strlen(const char *s);
void* mkrtos_memset(void * dst, int s, size_t count) ;

int mkrtos_isupper ( int ch ) ;
int mkrtos_islower( int ch ) ;
int mkrtos_tolower(int ch) ;
int mkrtos_toupper(int ch) ;
int mkrtos_iscntrl ( int ch ) ;
#endif /* INC_KNL_SERVICE_H_ */
