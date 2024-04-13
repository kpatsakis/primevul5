str_replace_shared(VALUE str2, VALUE str)
{
    if (RSTRING_LEN(str) <= RSTRING_EMBED_LEN_MAX) {
	STR_SET_EMBED(str2);
	memcpy(RSTRING_PTR(str2), RSTRING_PTR(str), RSTRING_LEN(str)+1);
	STR_SET_EMBED_LEN(str2, RSTRING_LEN(str));
    }
    else {
	FL_SET(str2, STR_NOEMBED);
	str = rb_str_new_frozen(str);
	RSTRING(str2)->as.heap.len = RSTRING_LEN(str);
	RSTRING(str2)->as.heap.ptr = RSTRING_PTR(str);
	RSTRING(str2)->as.heap.aux.shared = str;
	FL_SET(str2, ELTS_SHARED);
    }
    rb_enc_cr_str_exact_copy(str2, str);

    return str2;
}