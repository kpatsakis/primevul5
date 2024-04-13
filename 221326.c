rb_external_str_new_cstr(const char *ptr)
{
    return rb_external_str_new_with_enc(ptr, strlen(ptr), rb_default_external_encoding());
}