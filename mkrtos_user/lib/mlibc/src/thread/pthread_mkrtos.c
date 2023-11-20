#include <pthread_impl.h>
#include <u_types.h>

obj_handler_t pthread_hd_get(pthread_t th)
{
    return th->hd;
}
