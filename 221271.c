rb_str_length(VALUE str)
{
    long len;

    len = str_strlen(str, STR_ENC_GET(str));
    return LONG2NUM(len);
}