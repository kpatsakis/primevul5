rb_str_split_m(int argc, VALUE *argv, VALUE str)
{
    rb_encoding *enc;
    VALUE spat;
    VALUE limit;
    enum {awk, string, regexp} split_type;
    long beg, end, i = 0;
    int lim = 0;
    VALUE result, tmp;

    if (rb_scan_args(argc, argv, "02", &spat, &limit) == 2) {
	lim = NUM2INT(limit);
	if (lim <= 0) limit = Qnil;
	else if (lim == 1) {
	    if (RSTRING_LEN(str) == 0)
		return rb_ary_new2(0);
	    return rb_ary_new3(1, str);
	}
	i = 1;
    }

    enc = STR_ENC_GET(str);
    if (NIL_P(spat)) {
	if (!NIL_P(rb_fs)) {
	    spat = rb_fs;
	    goto fs_set;
	}
	split_type = awk;
    }
    else {
      fs_set:
	if (TYPE(spat) == T_STRING) {
	    rb_encoding *enc2 = STR_ENC_GET(spat);

	    split_type = string;
	    if (RSTRING_LEN(spat) == 0) {
		/* Special case - split into chars */
		spat = rb_reg_regcomp(spat);
		split_type = regexp;
	    }
	    else if (rb_enc_asciicompat(enc2) == 1) {
		if (RSTRING_LEN(spat) == 1 && RSTRING_PTR(spat)[0] == ' '){
		    split_type = awk;
		}
	    }
	    else {
		int l;
		if (rb_enc_ascget(RSTRING_PTR(spat), RSTRING_END(spat), &l, enc2) == ' ' &&
		    RSTRING_LEN(spat) == l) {
		    split_type = awk;
		}
	    }
	}
	else {
	    spat = get_pat(spat, 1);
	    split_type = regexp;
	}
    }

    result = rb_ary_new();
    beg = 0;
    if (split_type == awk) {
	char *ptr = RSTRING_PTR(str);
	char *eptr = RSTRING_END(str);
	char *bptr = ptr;
	int skip = 1;
	unsigned int c;

	end = beg;
	if (is_ascii_string(str)) {
	    while (ptr < eptr) {
		c = (unsigned char)*ptr++;
		if (skip) {
		    if (ascii_isspace(c)) {
			beg = ptr - bptr;
		    }
		    else {
			end = ptr - bptr;
			skip = 0;
			if (!NIL_P(limit) && lim <= i) break;
		    }
		}
		else if (ascii_isspace(c)) {
		    rb_ary_push(result, rb_str_subseq(str, beg, end-beg));
		    skip = 1;
		    beg = ptr - bptr;
		    if (!NIL_P(limit)) ++i;
		}
		else {
		    end = ptr - bptr;
		}
	    }
	}
	else {
	    while (ptr < eptr) {
		int n;

		c = rb_enc_codepoint_len(ptr, eptr, &n, enc);
		ptr += n;
		if (skip) {
		    if (rb_isspace(c)) {
			beg = ptr - bptr;
		    }
		    else {
			end = ptr - bptr;
			skip = 0;
			if (!NIL_P(limit) && lim <= i) break;
		    }
		}
		else if (rb_isspace(c)) {
		    rb_ary_push(result, rb_str_subseq(str, beg, end-beg));
		    skip = 1;
		    beg = ptr - bptr;
		    if (!NIL_P(limit)) ++i;
		}
		else {
		    end = ptr - bptr;
		}
	    }
	}
    }
    else if (split_type == string) {
	char *ptr = RSTRING_PTR(str);
	char *temp = ptr;
	char *eptr = RSTRING_END(str);
	char *sptr = RSTRING_PTR(spat);
	long slen = RSTRING_LEN(spat);

	if (is_broken_string(str)) {
	    rb_raise(rb_eArgError, "invalid byte sequence in %s", rb_enc_name(STR_ENC_GET(str)));
	}
	if (is_broken_string(spat)) {
	    rb_raise(rb_eArgError, "invalid byte sequence in %s", rb_enc_name(STR_ENC_GET(spat)));
	}
	enc = rb_enc_check(str, spat);
	while (ptr < eptr &&
	       (end = rb_memsearch(sptr, slen, ptr, eptr - ptr, enc)) >= 0) {
	    /* Check we are at the start of a char */
	    char *t = rb_enc_right_char_head(ptr, ptr + end, eptr, enc);
	    if (t != ptr + end) {
		ptr = t;
		continue;
	    }
	    rb_ary_push(result, rb_str_subseq(str, ptr - temp, end));
	    ptr += end + slen;
	    if (!NIL_P(limit) && lim <= ++i) break;
	}
	beg = ptr - temp;
    }
    else {
	char *ptr = RSTRING_PTR(str);
	long len = RSTRING_LEN(str);
	long start = beg;
	long idx;
	int last_null = 0;
	struct re_registers *regs;

	while ((end = rb_reg_search(spat, str, start, 0)) >= 0) {
	    regs = RMATCH_REGS(rb_backref_get());
	    if (start == end && BEG(0) == END(0)) {
		if (!ptr) {
		    rb_ary_push(result, rb_str_new("", 0));
		    break;
		}
		else if (last_null == 1) {
		    rb_ary_push(result, rb_str_subseq(str, beg,
						      rb_enc_fast_mbclen(ptr+beg,
									 ptr+len,
									 enc)));
		    beg = start;
		}
		else {
                    if (ptr+start == ptr+len)
                        start++;
                    else
                        start += rb_enc_fast_mbclen(ptr+start,ptr+len,enc);
		    last_null = 1;
		    continue;
		}
	    }
	    else {
		rb_ary_push(result, rb_str_subseq(str, beg, end-beg));
		beg = start = END(0);
	    }
	    last_null = 0;

	    for (idx=1; idx < regs->num_regs; idx++) {
		if (BEG(idx) == -1) continue;
		if (BEG(idx) == END(idx))
		    tmp = rb_str_new5(str, 0, 0);
		else
		    tmp = rb_str_subseq(str, BEG(idx), END(idx)-BEG(idx));
		rb_ary_push(result, tmp);
	    }
	    if (!NIL_P(limit) && lim <= ++i) break;
	}
    }
    if (RSTRING_LEN(str) > 0 && (!NIL_P(limit) || RSTRING_LEN(str) > beg || lim < 0)) {
	if (RSTRING_LEN(str) == beg)
	    tmp = rb_str_new5(str, 0, 0);
	else
	    tmp = rb_str_subseq(str, beg, RSTRING_LEN(str)-beg);
	rb_ary_push(result, tmp);
    }
    if (NIL_P(limit) && lim == 0) {
	long len;
	while ((len = RARRAY_LEN(result)) > 0 &&
	       (tmp = RARRAY_PTR(result)[len-1], RSTRING_LEN(tmp) == 0))
	    rb_ary_pop(result);
    }

    return result;
}