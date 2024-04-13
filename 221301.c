str_new4(VALUE klass, VALUE str)
{
    VALUE str2;

    str2 = str_alloc(klass);
    STR_SET_NOEMBED(str2);
    RSTRING(str2)->as.heap.len = RSTRING_LEN(str);
    RSTRING(str2)->as.heap.ptr = RSTRING_PTR(str);
    if (STR_SHARED_P(str)) {
	VALUE shared = RSTRING(str)->as.heap.aux.shared;
	assert(OBJ_FROZEN(shared));
	FL_SET(str2, ELTS_SHARED);
	RSTRING(str2)->as.heap.aux.shared = shared;
    }
    else {
	FL_SET(str, ELTS_SHARED);
	RSTRING(str)->as.heap.aux.shared = str2;
    }
    rb_enc_cr_str_exact_copy(str2, str);
    OBJ_INFECT(str2, str);
    return str2;
}