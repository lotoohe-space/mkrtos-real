#include "u_types.h"
#include "u_prot.h"
#include "u_log.h"
#include <stdio.h>
#include <CuTest.h>

static void ulog_test(CuTest *tc)
{
    uint8_t data[10];

    ulog_write_str(LOG_PROT, "Please key..\n");
#if 0
    int len = ulog_read_bytes(LOG_PROT, data, sizeof(data) - 1);
    if (len > 0)
    {
        data[len] = 0;
        printf("%s\n", data);
    }
#endif
}
static CuSuite suite;

CuSuite *ulog_test_suite(void)
{
    CuSuiteInit(&suite);

    SUITE_ADD_TEST(&suite, ulog_test);

    return &suite;
}