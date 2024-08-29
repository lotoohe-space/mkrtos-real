#pragma once
#include <CuTest.h>

CuSuite* printf_test_suite(void);
CuSuite *ulog_test_suite(void);
CuSuite *sema_test_suite(void);
CuSuite *vmm_test_suite(void);

CuSuite *malloc_test_suite(void);
CuSuite *pthread_press_test_suite(void);
CuSuite *pthread_cond_lock_test_suite(void);
CuSuite *pthread_lock_test_suite(void);
CuSuite *map_test_suite(void);
CuSuite *pthread_base_test_suite(void);
CuSuite *thread_base_test_suite(void);
CuSuite *ipc_test_suite(void);
CuSuite *sharem_mem_test_suite(void);
void test_main(void);

void mm_test(void);
void ulog_test(void);
void factory_test(void);
void app_test(void);
void mpu_test(void);
void printf_test(void);
void ipc_test(void);
void ipc_test2(void);
void thread_exit_test(void);
void map_test(void);
void ipc_obj_test(void);
void irq_test(void);
void kobj_create_press_test(void);
void sleep_tick(int tick);
void pthread_lock_test(void);
int pthread_cond_lock_test(void);
void sharea_mem_test(void);
void ns_test(void);
void malloc_test(void);
void thread_cpu_test(void);
int thread_vcpu_test(void);
void ipi_test(void);
int u_sema_test(void);
int u_sema_test2(void);
