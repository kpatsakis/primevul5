rb_str_shared_replace(VALUE str, VALUE str2)
{
    rb_encoding *enc;
    int cr;
    if (str == str2) return;
    enc = STR_ENC_GET(str2);
    cr = ENC_CODERANGE(str2);
    str_discard(str);
    OBJ_INFECT(str, str2);
    if (RSTRING_LEN(str2) <= RSTRING_EMBED_LEN_MAX) {
	STR_SET_EMBED(str);
	memcpy(RSTRING_PTR(str), RSTRING_PTR(str2), RSTRING_LEN(str2)+1);
	STR_SET_EMBED_LEN(str, RSTRING_LEN(str2));
        rb_enc_associate(str, enc);
        ENC_CODERANGE_SET(str, cr);
	return;
    }
    STR_SET_NOEMBED(str);
    STR_UNSET_NOCAPA(str);
    RSTRING(str)->as.heap.ptr = RSTRING_PTR(str2);
    RSTRING(str)->as.heap.len = RSTRING_LEN(str2);
    if (STR_NOCAPA_P(str2)) {
	FL_SET(str, RBASIC(str2)->flags & STR_NOCAPA);
	RSTRING(str)->as.heap.aux.shared = RSTRING(str2)->as.heap.aux.shared;
    }
    else {
	RSTRING(str)->as.heap.aux.capa = RSTRING(str2)->as.heap.aux.capa;
    }
    STR_SET_EMBED(str2);	/* abandon str2 */
    RSTRING_PTR(str2)[0] = 0;
    STR_SET_EMBED_LEN(str2, 0);
    rb_enc_associate(str, enc);
    ENC_CODERANGE_SET(str, cr);
}