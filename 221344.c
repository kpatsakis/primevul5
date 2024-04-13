rb_str_export_locale(VALUE str)
{
    return rb_str_conv_enc(str, STR_ENC_GET(str), rb_locale_encoding());
}