#include "u_log.h"
#include "ns_cli.h"
#include "u_rpc_svr.h"
#include "u_prot.h"
#include "u_env.h"
#include "cons_cli.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(int args, char *argv[])
{
    extern int core_mark(int argc, char *argv[]) ;

#if 1
    thread_run(-1, 3);
#endif
    core_mark(args, argv);
    return 0;
}
