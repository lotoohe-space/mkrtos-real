#ifndef __CONS_H__
#define __CONS_H__
typedef unsigned long obj_handler_t;
typedef unsigned char uint8_t;
typedef unsigned long umword_t;
#define WORD_BYTES (sizeof(void *))
#define WORD_BITS (sizeof(void *) * 8)
#define ROUND(a, b) (((a) / (b)) + (((a) % (b)) ? 1 : 0))          //!< a/b后的值向上取整
#define ROUND_UP(a, b) ROUND(a, b)                                 //!< a除b向上取整数
extern int mk_syscall(int nr, ...);
void ulog_write_bytes(obj_handler_t obj_inx, const uint8_t *data, int len);


#endif
