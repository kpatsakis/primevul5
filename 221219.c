str_replace(VALUE str, VALUE str2)
{
    long len;

    len = RSTRING_LEN(str2);
    if (STR_ASSOC_P(str2)) {
	str2 = rb_str_new4(str2);
    }
    if (STR_SHARED_P(str2)) {
	VALUE shared = RSTRING(str2)->as.heap.aux.shared;
	assert(OBJ_FROZEN(shared));
	STR_SET_NOEMBED(str);
	RSTRING(str)->as.heap.len = len;
	RSTRING(str)->as.heap.ptr = RSTRING_PTR(str2);
	FL_SET(str, ELTS_SHARED);
	FL_UNSET(str, STR_ASSOC);
	RSTRING(str)->as.heap.aux.shared = shared;
    }
    else {
	str_replace_shared(str, str2);
    }

    OBJ_INFECT(str, str2);
    rb_enc_cr_str_exact_copy(str, str2);
    return str;
}