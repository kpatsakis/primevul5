spell_check_msm(void)
{
    char_u	*p = p_msm;
    long	start = 0;
    long	incr = 0;
    long	added = 0;

    if (!VIM_ISDIGIT(*p))
	return FAIL;
    /* block count = (value * 1024) / SBLOCKSIZE (but avoid overflow)*/
    start = (getdigits(&p) * 10) / (SBLOCKSIZE / 102);
    if (*p != ',')
	return FAIL;
    ++p;
    if (!VIM_ISDIGIT(*p))
	return FAIL;
    incr = (getdigits(&p) * 102) / (SBLOCKSIZE / 10);
    if (*p != ',')
	return FAIL;
    ++p;
    if (!VIM_ISDIGIT(*p))
	return FAIL;
    added = getdigits(&p) * 1024;
    if (*p != NUL)
	return FAIL;

    if (start == 0 || incr == 0 || added == 0 || incr > start)
	return FAIL;

    compress_start = start;
    compress_inc = incr;
    compress_added = added;
    return OK;
}