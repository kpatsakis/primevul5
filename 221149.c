rb_str_append(VALUE str, VALUE str2)
{
    rb_encoding *enc;
    int cr, cr2;

    StringValue(str2);
    if (RSTRING_LEN(str2) > 0 && STR_ASSOC_P(str)) {
        long len = RSTRING_LEN(str)+RSTRING_LEN(str2);
        enc = rb_enc_check(str, str2);
        cr = ENC_CODERANGE(str);
        if ((cr2 = ENC_CODERANGE(str2)) > cr) cr = cr2;
        rb_str_modify(str);
        REALLOC_N(RSTRING(str)->as.heap.ptr, char, len+1);
        memcpy(RSTRING(str)->as.heap.ptr + RSTRING(str)->as.heap.len,
               RSTRING_PTR(str2), RSTRING_LEN(str2)+1);
        RSTRING(str)->as.heap.len = len;
        rb_enc_associate(str, enc);
        ENC_CODERANGE_SET(str, cr);
        OBJ_INFECT(str, str2);
        return str;
    }
    return rb_str_buf_append(str, str2);
}