#include "u_env.h"
#include "u_prot.h"
#include "u_hd_man.h"
static uenv_t env;

uenv_t *u_get_global_env(void)
{
    return &env;
}

void u_env_default_init(void)
{
    env.log_hd = LOG_PROT;
    env.ns_hd = HANDLER_INVALID;
    env.rev1 = HANDLER_INVALID;
    env.rev2 = HANDLER_INVALID;
}

void u_env_init(void *in_env)
{
    env = *((uenv_t *)in_env);
    hanlder_pre_alloc(env.log_hd);
    hanlder_pre_alloc(env.ns_hd);
    /*TODO:*/
}