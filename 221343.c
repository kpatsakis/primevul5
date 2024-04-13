rb_str_export_to_enc(VALUE str, rb_encoding *enc)
{
    return rb_str_conv_enc(str, STR_ENC_GET(str), enc);
}