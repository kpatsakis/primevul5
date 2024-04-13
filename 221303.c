str_cat_char(VALUE str, unsigned int c, rb_encoding *enc)
{
    char s[RUBY_MAX_CHAR_LEN];
    int n = rb_enc_codelen(c, enc);

    rb_enc_mbcput(c, s, enc);
    rb_enc_str_buf_cat(str, s, n, enc);
}