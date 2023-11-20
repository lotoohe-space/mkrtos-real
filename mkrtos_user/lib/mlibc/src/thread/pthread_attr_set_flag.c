#include "pthread_impl.h"

int pthread_attr_set_flag(pthread_attr_t *a, int flag)
{
    __acquire_ptc();
    a->_a_flag = PTHREAD_DONT_RUN;
    __release_ptc();
    return 0;
}
