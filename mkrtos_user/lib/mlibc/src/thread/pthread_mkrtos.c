#include <pthread_impl.h>
#include <u_types.h>

obj_handler_t pthread_hd_get(pthread_t th)
{
    return th->hd;
}

size_t pthread_struct_size(void)
{
    return sizeof(struct pthread);
}