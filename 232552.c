shorten_filenames(char_u **fnames, int count)
{
    int		i;
    char_u	dirname[MAXPATHL];
    char_u	*p;

    if (fnames == NULL || count < 1)
	return;
    mch_dirname(dirname, sizeof(dirname));
    for (i = 0; i < count; ++i)
    {
	if ((p = shorten_fname(fnames[i], dirname)) != NULL)
	{
	    /* shorten_fname() returns pointer in given "fnames[i]".  If free
	     * "fnames[i]" first, "p" becomes invalid.  So we need to copy
	     * "p" first then free fnames[i]. */
	    p = vim_strsave(p);
	    vim_free(fnames[i]);
	    fnames[i] = p;
	}
    }
}