chopped_length(VALUE str)
{
    rb_encoding *enc = STR_ENC_GET(str);
    const char *p, *p2, *beg, *end;

    beg = RSTRING_PTR(str);
    end = beg + RSTRING_LEN(str);
    if (beg > end) return 0;
    p = rb_enc_prev_char(beg, end, end, enc);
    if (!p) return 0;
    if (p > beg && rb_enc_ascget(p, end, 0, enc) == '\n') {
	p2 = rb_enc_prev_char(beg, p, end, enc);
	if (p2 && rb_enc_ascget(p2, end, 0, enc) == '\r') p = p2;
    }
    return p - beg;
}