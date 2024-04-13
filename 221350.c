str_nth(const char *p, const char *e, long nth, rb_encoding *enc, int singlebyte)
{
    if (singlebyte)
	p += nth;
    else {
	p = rb_enc_nth(p, e, nth, enc);
    }
    if (!p) return 0;
    if (p > e) p = e;
    return (char *)p;
}