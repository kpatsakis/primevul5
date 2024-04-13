write_spell_prefcond(FILE *fd, garray_T *gap)
{
    int		i;
    char_u	*p;
    int		len;
    int		totlen;
    size_t	x = 1;  /* collect return value of fwrite() */

    if (fd != NULL)
	put_bytes(fd, (long_u)gap->ga_len, 2);	    /* <prefcondcnt> */

    totlen = 2 + gap->ga_len; /* length of <prefcondcnt> and <condlen> bytes */

    for (i = 0; i < gap->ga_len; ++i)
    {
	/* <prefcond> : <condlen> <condstr> */
	p = ((char_u **)gap->ga_data)[i];
	if (p != NULL)
	{
	    len = (int)STRLEN(p);
	    if (fd != NULL)
	    {
		fputc(len, fd);
		x &= fwrite(p, (size_t)len, (size_t)1, fd);
	    }
	    totlen += len;
	}
	else if (fd != NULL)
	    fputc(0, fd);
    }

    return totlen;
}