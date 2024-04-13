rb_enc_str_new(const char *ptr, long len, rb_encoding *enc)
{
    VALUE str = rb_str_new(ptr, len);
    rb_enc_associate(str, enc);
    return str;
}