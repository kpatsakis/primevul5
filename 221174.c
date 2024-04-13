rb_str_associate(VALUE str, VALUE add)
{
    /* sanity check */
    if (OBJ_FROZEN(str)) rb_error_frozen("string");
    if (STR_ASSOC_P(str)) {
	/* already associated */
	rb_ary_concat(RSTRING(str)->as.heap.aux.shared, add);
    }
    else {
	if (STR_SHARED_P(str)) {
	    VALUE assoc = RSTRING(str)->as.heap.aux.shared;
	    str_make_independent(str);
	    if (STR_ASSOC_P(assoc)) {
		assoc = RSTRING(assoc)->as.heap.aux.shared;
		rb_ary_concat(assoc, add);
		add = assoc;
	    }
	}
	else if (STR_EMBED_P(str)) {
	    str_make_independent(str);
	}
	else if (RSTRING(str)->as.heap.aux.capa != RSTRING_LEN(str)) {
	    RESIZE_CAPA(str, RSTRING_LEN(str));
	}
	FL_SET(str, STR_ASSOC);
	RBASIC(add)->klass = 0;
	RSTRING(str)->as.heap.aux.shared = add;
    }
}