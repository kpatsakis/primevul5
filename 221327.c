rb_external_str_new(const char *ptr, long len)
{
    return rb_external_str_new_with_enc(ptr, len, rb_default_external_encoding());
}