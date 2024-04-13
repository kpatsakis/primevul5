readfile_charconvert(
    char_u	*fname,		/* name of input file */
    char_u	*fenc,		/* converted from */
    int		*fdp)		/* in/out: file descriptor of file */
{
    char_u	*tmpname;
    char_u	*errmsg = NULL;

    tmpname = vim_tempname('r', FALSE);
    if (tmpname == NULL)
	errmsg = (char_u *)_("Can't find temp file for conversion");
    else
    {
	close(*fdp);		/* close the input file, ignore errors */
	*fdp = -1;
	if (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = (char_u *)_("Conversion with 'charconvert' failed");
	if (errmsg == NULL && (*fdp = mch_open((char *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = (char_u *)_("can't read output of 'charconvert'");
    }

    if (errmsg != NULL)
    {
	/* Don't use emsg(), it breaks mappings, the retry with
	 * another type of conversion might still work. */
	MSG(errmsg);
	if (tmpname != NULL)
	{
	    mch_remove(tmpname);	/* delete converted file */
	    vim_free(tmpname);
	    tmpname = NULL;
	}
    }

    /* If the input file is closed, open it (caller should check for error). */
    if (*fdp < 0)
	*fdp = mch_open((char *)fname, O_RDONLY | O_EXTRA, 0);

    return tmpname;
}