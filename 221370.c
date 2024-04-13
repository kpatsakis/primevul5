rb_str_cmp(VALUE str1, VALUE str2)
{
    long len;
    int retval;

    len = lesser(RSTRING_LEN(str1), RSTRING_LEN(str2));
    retval = memcmp(RSTRING_PTR(str1), RSTRING_PTR(str2), len);
    if (retval == 0) {
	if (RSTRING_LEN(str1) == RSTRING_LEN(str2)) {
	    if (!rb_str_comparable(str1, str2)) {
		if (ENCODING_GET(str1) > ENCODING_GET(str2))
		    return 1;
		return -1;
	    }
	    return 0;
	}
	if (RSTRING_LEN(str1) > RSTRING_LEN(str2)) return 1;
	return -1;
    }
    if (retval > 0) return 1;
    return -1;
}