rb_str_clear(VALUE str)
{
    str_discard(str);
    STR_SET_EMBED(str);
    STR_SET_EMBED_LEN(str, 0);
    RSTRING_PTR(str)[0] = 0;
    if (rb_enc_asciicompat(STR_ENC_GET(str)))
	ENC_CODERANGE_SET(str, ENC_CODERANGE_7BIT);
    else
	ENC_CODERANGE_SET(str, ENC_CODERANGE_VALID);
    return str;
}