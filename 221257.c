trnext(struct tr *t, rb_encoding *enc)
{
    int n;

    for (;;) {
	if (!t->gen) {
	    if (t->p == t->pend) return -1;
	    if (t->p < t->pend - 1 && *t->p == '\\') {
		t->p++;
	    }
	    t->now = rb_enc_codepoint_len(t->p, t->pend, &n, enc);
	    t->p += n;
	    if (t->p < t->pend - 1 && *t->p == '-') {
		t->p++;
		if (t->p < t->pend) {
		    unsigned int c = rb_enc_codepoint_len(t->p, t->pend, &n, enc);
		    t->p += n;
		    if (t->now > c) {
			if (t->now < 0x80 && c < 0x80) {
			    rb_raise(rb_eArgError,
				     "invalid range \"%c-%c\" in string transliteration",
				     t->now, c);
			}
			else {
			    rb_raise(rb_eArgError, "invalid range in string transliteration");
			}
			continue; /* not reached */
		    }
		    t->gen = 1;
		    t->max = c;
		}
	    }
	    return t->now;
	}
	else if (++t->now < t->max) {
	    return t->now;
	}
	else {
	    t->gen = 0;
	    return t->max;
	}
    }
}