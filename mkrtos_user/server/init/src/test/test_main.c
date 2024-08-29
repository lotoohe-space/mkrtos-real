#include <stdio.h>

#include "CuTest.h"
#include "test.h"
static void RunAllTests(void)
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, ipc_test_suite());
	CuSuiteAddSuite(suite, ulog_test_suite());
	CuSuiteAddSuite(suite, printf_test_suite());
	CuSuiteAddSuite(suite, vmm_test_suite());
	CuSuiteAddSuite(suite, sharem_mem_test_suite());
	CuSuiteAddSuite(suite, map_test_suite());
	CuSuiteAddSuite(suite, thread_base_test_suite());
	CuSuiteAddSuite(suite, sema_test_suite());
	
	CuSuiteAddSuite(suite, malloc_test_suite());
	CuSuiteAddSuite(suite, pthread_base_test_suite());
	CuSuiteAddSuite(suite, pthread_press_test_suite());
	CuSuiteAddSuite(suite, pthread_lock_test_suite());
	CuSuiteAddSuite(suite, pthread_cond_lock_test_suite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}

void test_main(void)
{
    RunAllTests();
}