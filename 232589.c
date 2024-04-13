move_lines(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    int		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    /* Copy the lines in "frombuf" to "tobuf". */
    curbuf = tobuf;
    for (lnum = 1; lnum <= frombuf->b_ml.ml_line_count; ++lnum)
    {
	p = vim_strsave(ml_get_buf(frombuf, lnum, FALSE));
	if (p == NULL || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    break;
	}
	vim_free(p);
    }

    /* Delete all the lines in "frombuf". */
    if (retval != FAIL)
    {
	curbuf = frombuf;
	for (lnum = curbuf->b_ml.ml_line_count; lnum > 0; --lnum)
	    if (ml_delete(lnum, FALSE) == FAIL)
	    {
		/* Oops!  We could try putting back the saved lines, but that
		 * might fail again... */
		retval = FAIL;
		break;
	    }
    }

    curbuf = tbuf;
    return retval;
}