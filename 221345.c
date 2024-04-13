rb_str_to_s(VALUE str)
{
    if (rb_obj_class(str) != rb_cString) {
	return str_duplicate(rb_cString, str);
    }
    return str;
}