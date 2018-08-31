#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit.h"

#include "env.h"
#include "val.h"
#include "val_test.h"

static MunitResult
test_define_multi(const MunitParameter params[], void *fixture)
{
	struct env env;
	env_init(&env);

	val_t s = sym("foo");
	val_t v = sym("fooval");

	val_t res = env_define(&env, s, v);
	assert_val_eq(res, s);

	res = env_define(&env, s, v);
	assert_val_eq(res, err_undef());

	env_destroy(&env);

	return MUNIT_OK;
}

static MunitResult
test_define_lookup(const MunitParameter params[], void *fixture)
{
	struct env env;
	env_init(&env);

	val_t sym0 = sym("foo");
	val_t sym1 = sym("bar");
	val_t sym2 = sym("baz");

	val_t v0 = sym("fooval");
	val_t v1 = sym("barval");
	val_t v2 = sym("bazval");

	val_t res0;
	val_t res1;
	val_t res2;

	assert_val_eq(err_undef(), env_lookup(&env, sym0));
	assert_val_eq(err_undef(), env_lookup(&env, sym1));
	assert_val_eq(err_undef(), env_lookup(&env, sym2));

	env_define(&env, sym0, v0);
	res0 = env_lookup(&env, sym0);

	assert_val_eq(res0, v0);
	assert_val_eq(err_undef(), env_lookup(&env, sym1));
	assert_val_eq(err_undef(), env_lookup(&env, sym2));

	env_define(&env, sym1, v1);
	res1 = env_lookup(&env, sym1);

	assert_val_eq(res0, v0);
	assert_val_eq(res1, v1);
	assert_val_eq(err_undef(), env_lookup(&env, sym2));

	env_define(&env, sym2, v2);
	res2 = env_lookup(&env, sym2);

	assert_val_eq(res0, v0);
	assert_val_eq(res1, v1);
	assert_val_eq(res2, v2);

	env_destroy(&env);

	return MUNIT_OK;
}

/* TODO: test limit */

MunitTest env_tests[] = {
	{
		.name		= "/define-multi",
		.test		= test_define_multi,
		.setup		= NULL,
		.tear_down	= NULL,
		.options	= MUNIT_TEST_OPTION_NONE,
		.parameters	= NULL
	}, {
		.name		= "/define-lookup",
		.test		= test_define_lookup,
		.setup		= NULL,
		.tear_down	= NULL,
		.options	= MUNIT_TEST_OPTION_NONE,
		.parameters	= NULL
	}, {
		NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL
	}
};
