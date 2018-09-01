#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "val.h"

int
is_immed(val_t v)
{
	return get_storage(v) == VAL_STORAGE_IMMED;
}

int
is_boxed(val_t v)
{
	return get_storage(v) == VAL_STORAGE_BOXED;
}

val_t
quote(val_t v)
{
	return nonempty_list(builtin.sym.quote, nonempty_list(v, empty_list()));
}

val_t
unquote(val_t v)
{
	assert(is_quoted(v));

	return car(cdr(v));
}

int
is_quoted(val_t v)
{
	return is_nonempty_list(v) && is_eq(builtin.sym.quote, car(v));
}

int
is_eq(val_t v, val_t w)
{
	unsigned long v_storage = get_storage(v);

	unsigned long v_immed_type;
	unsigned long w_immed_type;

	unsigned long v_boxed_type;
	unsigned long w_boxed_type;

	const char *v_sym_name;
	const char *w_sym_name;

	switch (v_storage) {
	case VAL_STORAGE_IMMED:
		if (!is_immed(w))
			return 0;

		v_immed_type = get_immed_type(v);
		w_immed_type = get_immed_type(w);
		if (v_immed_type != w_immed_type)
			return 0;

		return 1;

	case VAL_STORAGE_BOXED:
		if (!is_boxed(w))
			return 0;

		v_boxed_type = get_boxed_type(v);
		w_boxed_type = get_boxed_type(w);
		if (v_boxed_type != w_boxed_type)
			return 0;

		switch (v_boxed_type) {
		case VAL_BOXED_TYPE_SYM:
			v_sym_name = sym_name(v);
			w_sym_name = sym_name(w);
			return v.u == w.u
			    && strcmp(v_sym_name, w_sym_name) == 0;

		case VAL_BOXED_TYPE_LIST:
			return nonempty_list_eq(v, w);

		default:
			break;
		}

	default:
		break;
	}

	assert(0 && "NOTREACHED");

	return 0;
}

void
val_free(val_t v)
{
	switch (get_storage(v)) {
	case VAL_STORAGE_IMMED:
		return;

	case VAL_STORAGE_BOXED:
		switch (get_boxed_type(v)) {
		case VAL_BOXED_TYPE_SYM:
			return;

		case VAL_BOXED_TYPE_LIST:
			nonempty_list_free(v);
			return;

		case VAL_BOXED_TYPE_LAMBDA:
			lambda_free(v);
			return;
		}
	}

	assert(0 && "NOTREACHED");
}
