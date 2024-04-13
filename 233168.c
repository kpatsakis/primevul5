read_sofo_section(FILE *fd, slang_T *slang)
{
    int		cnt;
    char_u	*from, *to;
    int		res;

    slang->sl_sofo = TRUE;

    /* <sofofromlen> <sofofrom> */
    from = read_cnt_string(fd, 2, &cnt);
    if (cnt < 0)
	return cnt;

    /* <sofotolen> <sofoto> */
    to = read_cnt_string(fd, 2, &cnt);
    if (cnt < 0)
    {
	vim_free(from);
	return cnt;
    }

    /* Store the info in slang->sl_sal and/or slang->sl_sal_first. */
    if (from != NULL && to != NULL)
	res = set_sofo(slang, from, to);
    else if (from != NULL || to != NULL)
	res = SP_FORMERROR;    /* only one of two strings is an error */
    else
	res = 0;

    vim_free(from);
    vim_free(to);
    return res;
}