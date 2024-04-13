rb_str_hash_cmp(VALUE str1, VALUE str2)
{
    long len;

    if (!rb_str_comparable(str1, str2)) return 1;
    if (RSTRING_LEN(str1) == (len = RSTRING_LEN(str2)) &&
	memcmp(RSTRING_PTR(str1), RSTRING_PTR(str2), len) == 0) {
	return 0;
    }
    return 1;
}