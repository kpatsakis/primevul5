readfile_linenr(
    linenr_T	linecnt,	/* line count before reading more bytes */
    char_u	*p,		/* start of more bytes read */
    char_u	*endp)		/* end of more bytes read */
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_line_count - linecnt + 1;
    for (s = p; s < endp; ++s)
	if (*s == '\n')
	    ++lnum;
    return lnum;
}