has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    int		retval = FALSE;

    fname = FullName_save(sfname, FALSE);
    if (fname == NULL)
	return FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    if (sfname != NULL)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    for (ap = first_autopat[(int)event]; ap != NULL; ap = ap->next)
	if (ap->pat != NULL && ap->cmds != NULL
	      && (ap->buflocal_nr == 0
		? match_file_pat(NULL, &ap->reg_prog,
					  fname, sfname, tail, ap->allow_dirs)
		: buf != NULL && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    break;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    return retval;
}