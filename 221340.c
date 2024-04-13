str_modifiable(VALUE str)
{
    if (FL_TEST(str, STR_TMPLOCK)) {
	rb_raise(rb_eRuntimeError, "can't modify string; temporarily locked");
    }
    if (OBJ_FROZEN(str)) rb_error_frozen("string");
    if (!OBJ_UNTRUSTED(str) && rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't modify string");
}