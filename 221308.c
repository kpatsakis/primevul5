rb_str_succ(VALUE orig)
{
    rb_encoding *enc;
    VALUE str;
    char *sbeg, *s, *e, *last_alnum = 0;
    int c = -1;
    long l;
    char carry[ONIGENC_CODE_TO_MBC_MAXLEN] = "\1";
    long carry_pos = 0, carry_len = 1;
    enum neighbor_char neighbor = NEIGHBOR_FOUND;

    str = rb_str_new5(orig, RSTRING_PTR(orig), RSTRING_LEN(orig));
    rb_enc_cr_str_copy_for_substr(str, orig);
    OBJ_INFECT(str, orig);
    if (RSTRING_LEN(str) == 0) return str;

    enc = STR_ENC_GET(orig);
    sbeg = RSTRING_PTR(str);
    s = e = sbeg + RSTRING_LEN(str);

    while ((s = rb_enc_prev_char(sbeg, s, e, enc)) != 0) {
	if (neighbor == NEIGHBOR_NOT_CHAR && last_alnum) {
	    if (ISALPHA(*last_alnum) ? ISDIGIT(*s) :
		ISDIGIT(*last_alnum) ? ISALPHA(*s) : 0) {
		s = last_alnum;
		break;
	    }
	}
	if ((l = rb_enc_precise_mbclen(s, e, enc)) <= 0) continue;
        neighbor = enc_succ_alnum_char(s, l, enc, carry);
        switch (neighbor) {
	  case NEIGHBOR_NOT_CHAR:
	    continue;
	  case NEIGHBOR_FOUND:
	    return str;
	  case NEIGHBOR_WRAPPED:
	    last_alnum = s;
	    break;
	}
        c = 1;
        carry_pos = s - sbeg;
        carry_len = l;
    }
    if (c == -1) {		/* str contains no alnum */
	s = e;
	while ((s = rb_enc_prev_char(sbeg, s, e, enc)) != 0) {
            enum neighbor_char neighbor;
            if ((l = rb_enc_precise_mbclen(s, e, enc)) <= 0) continue;
            neighbor = enc_succ_char(s, l, enc);
            if (neighbor == NEIGHBOR_FOUND)
                return str;
            if (rb_enc_precise_mbclen(s, s+l, enc) != l) {
                /* wrapped to \0...\0.  search next valid char. */
                enc_succ_char(s, l, enc);
            }
            if (!rb_enc_asciicompat(enc)) {
                MEMCPY(carry, s, char, l);
                carry_len = l;
            }
            carry_pos = s - sbeg;
	}
    }
    RESIZE_CAPA(str, RSTRING_LEN(str) + carry_len);
    s = RSTRING_PTR(str) + carry_pos;
    memmove(s + carry_len, s, RSTRING_LEN(str) - carry_pos);
    memmove(s, carry, carry_len);
    STR_SET_LEN(str, RSTRING_LEN(str) + carry_len);
    RSTRING_PTR(str)[RSTRING_LEN(str)] = '\0';
    rb_enc_str_coderange(str);
    return str;
}