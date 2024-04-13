rb_str_chop(VALUE str)
{
    VALUE str2 = rb_str_new5(str, RSTRING_PTR(str), chopped_length(str));
    rb_enc_cr_str_copy_for_substr(str2, str);
    OBJ_INFECT(str2, str);
    return str2;
}