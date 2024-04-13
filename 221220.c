rb_str_sublen(VALUE str, long pos)
{
    if (single_byte_optimizable(str) || pos < 0)
        return pos;
    else {
	char *p = RSTRING_PTR(str);
        return enc_strlen(p, p + pos, STR_ENC_GET(str), ENC_CODERANGE(str));
    }
}