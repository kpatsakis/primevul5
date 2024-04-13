str_enc_copy(VALUE str1, VALUE str2)
{
    rb_enc_set_index(str1, ENCODING_GET(str2));
}