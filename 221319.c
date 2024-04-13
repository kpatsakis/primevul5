rb_str_dup(VALUE str)
{
    return str_duplicate(rb_obj_class(str), str);
}