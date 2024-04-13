rb_str_export(VALUE str)
{
    return rb_str_conv_enc(str, STR_ENC_GET(str), rb_default_external_encoding());
}