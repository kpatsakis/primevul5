str_modify_keep_cr(VALUE str)
{
    if (!str_independent(str))
	str_make_independent(str);
    if (ENC_CODERANGE(str) == ENC_CODERANGE_BROKEN)
	/* Force re-scan later */
	ENC_CODERANGE_CLEAR(str);
}