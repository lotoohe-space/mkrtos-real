#include <stdio.h>

#include "CuTest.h"
#include "test.h"
#include <stdlib.h>
#include <u_util.h>
#include <u_sleep.h>
static CuSuite suite;
static void RunAllTests(void)
{
	CuString *output = CuStringNew();
	if (output == NULL)
	{
		printf("malloc failed\n");
		exit(1);
	}
	CuSuiteInit(&suite);
#if 0
	CuSuiteAddSuite(&suite, ipc_test_suite());
#endif
	CuSuiteAddSuite(&suite, ulog_test_suite());
	CuSuiteAddSuite(&suite, printf_test_suite());
	CuSuiteAddSuite(&suite, vmm_test_suite());
	CuSuiteAddSuite(&suite, sharem_mem_test_suite());
#if 0
	CuSuiteAddSuite(&suite, map_test_suite());
#endif
	CuSuiteAddSuite(&suite, thread_base_test_suite());
	CuSuiteAddSuite(&suite, sema_test_suite());
	CuSuiteAddSuite(&suite, pthread_base_test_suite());
	CuSuiteAddSuite(&suite, pthread_press_test_suite());
	CuSuiteAddSuite(&suite, pthread_lock_test_suite());
	CuSuiteAddSuite(&suite, pthread_cond_lock_test_suite());
	CuSuiteAddSuite(&suite, malloc_test_suite());

	CuSuiteRun(&suite);
	CuSuiteSummary(&suite, output);
	CuSuiteDetails(&suite, output);
	printf("%s\n", output->buffer);
	u_sleep_ms(3000);
	printf("MKRTOS test ok.\n");
}

void test_main(void)
{
	RunAllTests();
	while (1)
	{
		u_sleep_ms(1000);
	}
}