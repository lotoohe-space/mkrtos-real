/*
 * mkrtos_strncpy.c
 *
 *  Created on: 2022��8��6��
 *      Author: Administrator
 */


#include <knl_service.h>
char *mkrtos_strncpy(char *dest, const char *src, uint32_t n) {
  mkrtos_memccpy(dest,src,0,n);
  return dest;
}
