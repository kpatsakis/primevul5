rb_str_new_frozen(VALUE orig)
{
    VALUE klass, str;

    if (OBJ_FROZEN(orig)) return orig;
    klass = rb_obj_class(orig);
    if (STR_SHARED_P(orig) && (str = RSTRING(orig)->as.heap.aux.shared)) {
	long ofs;
	assert(OBJ_FROZEN(str));
	ofs = RSTRING_LEN(str) - RSTRING_LEN(orig);
	if ((ofs > 0) || (klass != RBASIC(str)->klass) ||
	    (!OBJ_TAINTED(str) && OBJ_TAINTED(orig)) ||
	    ENCODING_GET(str) != ENCODING_GET(orig)) {
	    str = str_new3(klass, str);
	    RSTRING(str)->as.heap.ptr += ofs;
	    RSTRING(str)->as.heap.len -= ofs;
	    rb_enc_cr_str_exact_copy(str, orig);
	    OBJ_INFECT(str, orig);
	}
    }
    else if (STR_EMBED_P(orig)) {
	str = str_new(klass, RSTRING_PTR(orig), RSTRING_LEN(orig));
	rb_enc_cr_str_exact_copy(str, orig);
	OBJ_INFECT(str, orig);
    }
    else if (STR_ASSOC_P(orig)) {
	VALUE assoc = RSTRING(orig)->as.heap.aux.shared;
	FL_UNSET(orig, STR_ASSOC);
	str = str_new4(klass, orig);
	FL_SET(str, STR_ASSOC);
	RSTRING(str)->as.heap.aux.shared = assoc;
    }
    else {
	str = str_new4(klass, orig);
    }
    OBJ_FREEZE(str);
    return str;
}