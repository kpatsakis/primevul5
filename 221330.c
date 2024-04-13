str_frozen_check(VALUE s)
{
    if (OBJ_FROZEN(s)) {
	rb_raise(rb_eRuntimeError, "string frozen");
    }
}