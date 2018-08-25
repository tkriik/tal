#include <stddef.h>

#include "munit.h"

extern MunitTest parse_tests[];
extern MunitTest sym_tests[];
extern MunitTest token_tests[];
extern MunitTest val_tests[];

static MunitSuite suites[] = {
	{
		.prefix		= "/parse",
		.tests		= parse_tests,
		.suites		= NULL,
		.iterations	= 1,
		.options	= MUNIT_SUITE_OPTION_NONE
	}, {
		.prefix		= "/sym",
		.tests		= sym_tests,
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
		.prefix		= "/val",
		.tests		= val_tests,
		.suites		= NULL,
		.iterations	= 1,
		.options	= MUNIT_SUITE_OPTION_NONE
	},
	{ NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
};

static const MunitSuite suite = {
	.prefix		= "/tal",
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
