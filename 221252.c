rb_filesystem_str_new(const char *ptr, long len)
{
    return rb_external_str_new_with_enc(ptr, len, rb_filesystem_encoding());
}