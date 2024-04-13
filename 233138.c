read_rep_section(FILE *fd, garray_T *gap, short *first)
{
    int		cnt;
    fromto_T	*ftp;
    int		i;

    cnt = get2c(fd);					/* <repcount> */
    if (cnt < 0)
	return SP_TRUNCERROR;

    if (ga_grow(gap, cnt) == FAIL)
	return SP_OTHERERROR;

    /* <rep> : <repfromlen> <repfrom> <reptolen> <repto> */
    for (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ftp = &((fromto_T *)gap->ga_data)[gap->ga_len];
	ftp->ft_from = read_cnt_string(fd, 1, &i);
	if (i < 0)
	    return i;
	if (i == 0)
	    return SP_FORMERROR;
	ftp->ft_to = read_cnt_string(fd, 1, &i);
	if (i <= 0)
	{
	    vim_free(ftp->ft_from);
	    if (i < 0)
		return i;
	    return SP_FORMERROR;
	}
    }

    /* Fill the first-index table. */
    for (i = 0; i < 256; ++i)
	first[i] = -1;
    for (i = 0; i < gap->ga_len; ++i)
    {
	ftp = &((fromto_T *)gap->ga_data)[i];
	if (first[*ftp->ft_from] == -1)
	    first[*ftp->ft_from] = i;
    }
    return 0;
}