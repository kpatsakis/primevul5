rb_usascii_str_new_cstr(const char *ptr)
{
    VALUE str = rb_str_new2(ptr);
    ENCODING_CODERANGE_SET(str, rb_usascii_encindex(), ENC_CODERANGE_7BIT);
    return str;
}