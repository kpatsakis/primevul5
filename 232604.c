shorten_fnames(int force)
{
    char_u	dirname[MAXPATHL];
    buf_T	*buf;
    char_u	*p;

    mch_dirname(dirname, MAXPATHL);
    FOR_ALL_BUFFERS(buf)
    {
	if (buf->b_fname != NULL
#ifdef FEAT_QUICKFIX
		&& !bt_nofile(buf)
#endif
		&& !path_with_url(buf->b_fname)
		&& (force
		    || buf->b_sfname == NULL
		    || mch_isFullName(buf->b_sfname)))
	{
	    vim_free(buf->b_sfname);
	    buf->b_sfname = NULL;
	    p = shorten_fname(buf->b_ffname, dirname);
	    if (p != NULL)
	    {
		buf->b_sfname = vim_strsave(p);
		buf->b_fname = buf->b_sfname;
	    }
	    if (p == NULL || buf->b_fname == NULL)
		buf->b_fname = buf->b_ffname;
	}

	/* Always make the swap file name a full path, a "nofile" buffer may
	 * also have a swap file. */
	mf_fullname(buf->b_ml.ml_mfp);
    }
    status_redraw_all();
    redraw_tabline = TRUE;
}