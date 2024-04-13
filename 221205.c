str_utf8_nth(const char *p, const char *e, long nth)
{
    if ((int)SIZEOF_VALUE < e - p && (int)SIZEOF_VALUE * 2 < nth) {
	const VALUE *s, *t;
	const VALUE lowbits = sizeof(VALUE) - 1;
	s = (const VALUE*)(~lowbits & ((VALUE)p + lowbits));
	t = (const VALUE*)(~lowbits & (VALUE)e);
	while (p < (const char *)s) {
	    if (is_utf8_lead_byte(*p)) nth--;
	    p++;
	}
	do {
	    nth -= count_utf8_lead_bytes_with_word(s);
	    s++;
	} while (s < t && (int)sizeof(VALUE) <= nth);
	p = (char *)s;
    }
    while (p < e) {
	if (is_utf8_lead_byte(*p)) {
	    if (nth == 0) break;
	    nth--;
	}
	p++;
    }
    return (char *)p;
}