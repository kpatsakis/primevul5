rb_str_set_len(VALUE str, long len)
{
    STR_SET_LEN(str, len);
    RSTRING_PTR(str)[len] = '\0';
}