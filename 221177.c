rb_str_plus(VALUE str1, VALUE str2)
{
    VALUE str3;
    rb_encoding *enc;

    StringValue(str2);
    enc = rb_enc_check(str1, str2);
    str3 = rb_str_new(0, RSTRING_LEN(str1)+RSTRING_LEN(str2));
    memcpy(RSTRING_PTR(str3), RSTRING_PTR(str1), RSTRING_LEN(str1));
    memcpy(RSTRING_PTR(str3) + RSTRING_LEN(str1),
	   RSTRING_PTR(str2), RSTRING_LEN(str2));
    RSTRING_PTR(str3)[RSTRING_LEN(str3)] = '\0';

    if (OBJ_TAINTED(str1) || OBJ_TAINTED(str2))
	OBJ_TAINT(str3);
    ENCODING_CODERANGE_SET(str3, rb_enc_to_index(enc),
			   ENC_CODERANGE_AND(ENC_CODERANGE(str1), ENC_CODERANGE(str2)));
    return str3;
}