rb_str_ord(VALUE s)
{
    unsigned int c;

    c = rb_enc_codepoint(RSTRING_PTR(s), RSTRING_END(s), STR_ENC_GET(s));
    return UINT2NUM(c);
}