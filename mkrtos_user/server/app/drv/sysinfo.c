#include "sysinfo.h"
#include <pthread.h>
sys_info_t sys_info = {0};

static pthread_spinlock_t lock;

void sys_info_lock(void)
{
    pthread_spin_lock(&lock);
}
void sys_info_unlock(void)
{
    pthread_spin_unlock(&lock);
}
