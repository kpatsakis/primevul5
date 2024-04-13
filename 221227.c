rb_str_freeze(VALUE str)
{
    if (STR_ASSOC_P(str)) {
	VALUE ary = RSTRING(str)->as.heap.aux.shared;
	OBJ_FREEZE(ary);
    }
    return rb_obj_freeze(str);
}