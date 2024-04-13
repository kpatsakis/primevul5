rb_str_times(VALUE str, VALUE times)
{
    VALUE str2;
    long n, len;
    char *ptr2;

    len = NUM2LONG(times);
    if (len < 0) {
	rb_raise(rb_eArgError, "negative argument");
    }
    if (len && LONG_MAX/len <  RSTRING_LEN(str)) {
	rb_raise(rb_eArgError, "argument too big");
    }

    str2 = rb_str_new5(str, 0, len *= RSTRING_LEN(str));
    ptr2 = RSTRING_PTR(str2);
    if (len) {
        n = RSTRING_LEN(str);
        memcpy(ptr2, RSTRING_PTR(str), n);
        while (n <= len/2) {
            memcpy(ptr2 + n, ptr2, n);
            n *= 2;
        }
        memcpy(ptr2 + n, ptr2, len-n);
    }
    ptr2[RSTRING_LEN(str2)] = '\0';
    OBJ_INFECT(str2, str);
    rb_enc_cr_str_copy_for_substr(str2, str);

    return str2;
}