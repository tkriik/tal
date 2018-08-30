#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "val.h"

val_t
_undef(void)
{
	val_t v;
	v.u = 0;

	return v;
}

val_t
nil(void)
{
	val_t v = _undef();

	_set_immed_nil(&v);

	return v;
}

int
is_immed(val_t v)
{
	return _get_storage(v) == VAL_STORAGE_IMMED;
}

int
is_boxed(val_t v)
{
	return _get_storage(v) == VAL_STORAGE_BOXED;
}

int
_is_undef(val_t v)
{
	return _get_storage(v) == VAL_STORAGE_IMMED
	    && _get_immed_type(v) == VAL_IMMED_TYPE_UNDEF;
}

int
is_nil(val_t v)
{
	return _get_storage(v) == VAL_STORAGE_IMMED
	    && _get_immed_type(v) == VAL_IMMED_TYPE_NIL;
}

int
is_eq(val_t v, val_t w)
{
	unsigned long v_storage = _get_storage(v);

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

		v_immed_type = _get_immed_type(v);
		w_immed_type = _get_immed_type(w);
		if (v_immed_type != w_immed_type)
			return 0;

		return 1;

	case VAL_STORAGE_BOXED:
		if (!is_boxed(w))
			return 0;

		v_boxed_type = _get_boxed_type(v);
		w_boxed_type = _get_boxed_type(w);
		if (v_boxed_type != w_boxed_type)
			return 0;

		switch (v_boxed_type) {
		case VAL_BOXED_TYPE_SYM:
			v_sym_name = sym_name(v);
			w_sym_name = sym_name(w);
			return strcmp(v_sym_name, w_sym_name) == 0;

		case VAL_BOXED_TYPE_LIST:
			return _blist_eq(v, w);

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
	switch (_get_storage(v)) {
	case VAL_STORAGE_IMMED:
		return;

	case VAL_STORAGE_BOXED:
		switch (_get_boxed_type(v)) {
		case VAL_BOXED_TYPE_SYM:
			return;

		case VAL_BOXED_TYPE_LIST:
			_blist_free(v);
			return;
		}
	}

	assert(0 && "NOTREACHED");
}
