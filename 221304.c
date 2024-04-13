rb_str_subseq(VALUE str, long beg, long len)
{
    VALUE str2;

    if (RSTRING_LEN(str) == beg + len &&
        RSTRING_EMBED_LEN_MAX < len) {
        str2 = rb_str_new_shared(rb_str_new_frozen(str));
        rb_str_drop_bytes(str2, beg);
    }
    else {
        str2 = rb_str_new5(str, RSTRING_PTR(str)+beg, len);
    }

    rb_enc_cr_str_copy_for_substr(str2, str);
    OBJ_INFECT(str2, str);

    return str2;
}