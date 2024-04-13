tr_find(unsigned int c, char table[256], VALUE del, VALUE nodel)
{
    if (c < 256) {
	return table[c] != 0;
    }
    else {
	VALUE v = UINT2NUM(c);

	if (del && !NIL_P(rb_hash_lookup(del, v))) {
	    if (!nodel || NIL_P(rb_hash_lookup(nodel, v))) {
		return TRUE;
	    }
	}
	return FALSE;
    }
}