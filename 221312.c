str_eql(const VALUE str1, const VALUE str2)
{
    const long len = RSTRING_LEN(str1);

    if (len != RSTRING_LEN(str2)) return Qfalse;
    if (!rb_str_comparable(str1, str2)) return Qfalse;
    if (memcmp(RSTRING_PTR(str1), RSTRING_PTR(str2), len) == 0)
	return Qtrue;
    return Qfalse;
}