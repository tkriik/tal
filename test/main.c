#include <stddef.h>

#include "munit.h"

extern MunitTest env_tests[];
extern MunitTest eval_tests[];
extern MunitTest lambda_tests[];
extern MunitTest parse_tests[];
extern MunitTest token_tests[];
extern MunitTest sval_tests[];
extern MunitTest sval_sym_tests[];
extern MunitTest sval_list_tests[];

static MunitSuite suites[] = {
	{
		.prefix		= "/val",
		.tests		= sval_tests,
		.suites		= NULL,
		.iterations	= 1,
		.options	= MUNIT_SUITE_OPTION_NONE
	}, {
		.prefix		= "/val/sym",
		.tests		= sval_sym_tests,
		.suites		= NULL,
		.iterations	= 1,
		.options	= MUNIT_SUITE_OPTION_NONE
	}, {
		.prefix		= "/val/list",
		.tests		= sval_list_tests,
		.suites		= NULL,
		.iterations	= 1,
		.options	= MUNIT_SUITE_OPTION_NONE
	}, {
		.prefix		= "/val/lambda",
		.tests		= lambda_tests,
		.suites		= NULL,
		.iterations	= 1,
		.options	= MUNIT_SUITE_OPTION_NONE
	}, {
		.prefix		= "/env",
		.tests		= env_tests,
		.suites		= NULL,
		.iterations	= 1,
		.options	= MUNIT_SUITE_OPTION_NONE
	}, {
		.prefix		= "/token",
		.tests		= token_tests,
		.suites		= NULL,
		.iterations	= 1,
		.options	= MUNIT_SUITE_OPTION_NONE
	}, {
		.prefix		= "/parse",
		.tests		= parse_tests,
		.suites		= NULL,
		.iterations	= 1,
		.options	= MUNIT_SUITE_OPTION_NONE
	}, {
		.prefix		= "/eval",
		.tests		= eval_tests,
		.suites		= NULL,
		.iterations	= 1,
		.options	= MUNIT_SUITE_OPTION_NONE
	}, {
		NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE
	}
};

static const MunitSuite suite = {
	.prefix		= "",
	.tests		= NULL,
	.suites		= suites,
	.iterations	= 1,
	.options	= MUNIT_SUITE_OPTION_NONE
};

int
main(int argc, char *argv[])
{
	return munit_suite_main(&suite, NULL, argc, argv);
}