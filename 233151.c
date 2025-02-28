read_cnt_string(FILE *fd, int cnt_bytes, int *cntp)
{
    int		cnt = 0;
    int		i;
    char_u	*str;

    /* read the length bytes, MSB first */
    for (i = 0; i < cnt_bytes; ++i)
	cnt = (cnt << 8) + getc(fd);
    if (cnt < 0)
    {
	*cntp = SP_TRUNCERROR;
	return NULL;
    }
    *cntp = cnt;
    if (cnt == 0)
	return NULL;	    /* nothing to read, return NULL */

    str = read_string(fd, cnt);
    if (str == NULL)
	*cntp = SP_OTHERERROR;
    return str;
}