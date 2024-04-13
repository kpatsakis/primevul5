set_rw_fname(char_u *fname, char_u *sfname)
{
#ifdef FEAT_AUTOCMD
    buf_T	*buf = curbuf;

    /* It's like the unnamed buffer is deleted.... */
    if (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, NULL, NULL, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, NULL, NULL, FALSE, curbuf);
# ifdef FEAT_EVAL
    if (aborting())	    /* autocmds may abort script processing */
	return FAIL;
# endif
    if (curbuf != buf)
    {
	/* We are in another buffer now, don't do the renaming. */
	EMSG(_(e_auchangedbuf));
	return FAIL;
    }
#endif

    if (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

#ifdef FEAT_AUTOCMD
    /* ....and a new named one is created */
    apply_autocmds(EVENT_BUFNEW, NULL, NULL, FALSE, curbuf);
    if (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, NULL, NULL, FALSE, curbuf);
# ifdef FEAT_EVAL
    if (aborting())	    /* autocmds may abort script processing */
	return FAIL;
# endif

    /* Do filetype detection now if 'filetype' is empty. */
    if (*curbuf->b_p_ft == NUL)
    {
	if (au_has_group((char_u *)"filetypedetect"))
	    (void)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, NULL);
	do_modelines(0);
    }
#endif

    return OK;
}