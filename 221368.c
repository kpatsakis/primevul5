rb_str_associated(VALUE str)
{
    if (STR_SHARED_P(str)) str = RSTRING(str)->as.heap.aux.shared;
    if (STR_ASSOC_P(str)) {
	return RSTRING(str)->as.heap.aux.shared;
    }
    return Qfalse;
}