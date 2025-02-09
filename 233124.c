read_region_section(FILE *fd, slang_T *lp, int len)
{
    int		i;

    if (len > 16)
	return SP_FORMERROR;
    for (i = 0; i < len; ++i)
	lp->sl_regions[i] = getc(fd);			/* <regionname> */
    lp->sl_regions[len] = NUL;
    return 0;
}