rb_str_casecmp(VALUE str1, VALUE str2)
{
    long len;
    rb_encoding *enc;
    char *p1, *p1end, *p2, *p2end;

    StringValue(str2);
    enc = rb_enc_compatible(str1, str2);
    if (!enc) {
	return Qnil;
    }

    p1 = RSTRING_PTR(str1); p1end = RSTRING_END(str1);
    p2 = RSTRING_PTR(str2); p2end = RSTRING_END(str2);
    if (single_byte_optimizable(str1) && single_byte_optimizable(str2)) {
	while (p1 < p1end && p2 < p2end) {
	    if (*p1 != *p2) {
		unsigned int c1 = TOUPPER(*p1 & 0xff);
		unsigned int c2 = TOUPPER(*p2 & 0xff);
                if (c1 != c2)
                    return INT2FIX(c1 < c2 ? -1 : 1);
	    }
	    p1++;
	    p2++;
	}
    }
    else {
	while (p1 < p1end && p2 < p2end) {
            int l1, c1 = rb_enc_ascget(p1, p1end, &l1, enc);
            int l2, c2 = rb_enc_ascget(p2, p2end, &l2, enc);

            if (0 <= c1 && 0 <= c2) {
                c1 = TOUPPER(c1);
                c2 = TOUPPER(c2);
                if (c1 != c2)
                    return INT2FIX(c1 < c2 ? -1 : 1);
            }
            else {
                int r;
                l1 = rb_enc_mbclen(p1, p1end, enc);
                l2 = rb_enc_mbclen(p2, p2end, enc);
                len = l1 < l2 ? l1 : l2;
                r = memcmp(p1, p2, len);
                if (r != 0)
                    return INT2FIX(r < 0 ? -1 : 1);
                if (l1 != l2)
                    return INT2FIX(l1 < l2 ? -1 : 1);
            }
	    p1 += l1;
	    p2 += l2;
	}
    }
    if (RSTRING_LEN(str1) == RSTRING_LEN(str2)) return INT2FIX(0);
    if (RSTRING_LEN(str1) > RSTRING_LEN(str2)) return INT2FIX(1);
    return INT2FIX(-1);
}