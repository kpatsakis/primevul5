rb_str_buf_append(VALUE str, VALUE str2)
{
    int str2_cr;

    str2_cr = ENC_CODERANGE(str2);

    rb_enc_cr_str_buf_cat(str, RSTRING_PTR(str2), RSTRING_LEN(str2),
        ENCODING_GET(str2), str2_cr, &str2_cr);

    OBJ_INFECT(str, str2);
    ENC_CODERANGE_SET(str2, str2_cr);

    return str;
}