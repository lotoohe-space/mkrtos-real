#ifndef _MKRTOS_SU_H_
#define _MKRTOS_SU_H_

extern int fcall(int call_no, int arg0,int arg1, int arg2);
extern void fcall_exit(void);
extern int slot_reg(char* func_name, void *fc, void *exit_fc);

#endif