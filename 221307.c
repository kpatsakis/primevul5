rb_str_new_shared(VALUE str)
{
    VALUE str2 = str_new3(rb_obj_class(str), str);

    OBJ_INFECT(str2, str);
    return str2;
}