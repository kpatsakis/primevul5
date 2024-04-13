changedir_func(
	char_u		*new_dir,
	int		forceit,
	cdscope_T	scope)
{
    char_u	*tofree;
    char_u	*pdir = NULL;
    int		dir_differs;
    int		retval = FALSE;

    if (new_dir == NULL || allbuf_locked())
	return FALSE;

    if (vim_strchr(p_cpo, CPO_CHDIR) != NULL && curbufIsChanged() && !forceit)
    {
	emsg(_("E747: Cannot change directory, buffer is modified (add ! to override)"));
	return FALSE;
    }

    // ":cd -": Change to previous directory
    if (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	if (pdir == NULL)
	{
	    emsg(_("E186: No previous directory"));
	    return FALSE;
	}
	new_dir = pdir;
    }

    // Free the previous directory
    tofree = get_prevdir(scope);

    // Save current directory for next ":cd -"
    if (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    else
	pdir = NULL;
    if (scope == CDSCOPE_WINDOW)
	curwin->w_prevdir = pdir;
    else if (scope == CDSCOPE_TABPAGE)
	curtab->tp_prevdir = pdir;
    else
	prev_dir = pdir;

#if defined(UNIX) || defined(VMS)
    // for UNIX ":cd" means: go to home directory
    if (*new_dir == NUL)
    {
	// use NameBuff for home directory name
# ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	if (p == NULL || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	else
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
# else
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
# endif
	new_dir = NameBuff;
    }
#endif
    dir_differs = new_dir == NULL || pdir == NULL
	|| pathcmp((char *)pdir, (char *)new_dir, -1) != 0;
    if (new_dir == NULL || (dir_differs && vim_chdir(new_dir)))
	emsg(_(e_failed));
    else
    {
	char_u  *acmd_fname;

	post_chdir(scope);

	if (dir_differs)
	{
	    if (scope == CDSCOPE_WINDOW)
		acmd_fname = (char_u *)"window";
	    else if (scope == CDSCOPE_TABPAGE)
		acmd_fname = (char_u *)"tabpage";
	    else
		acmd_fname = (char_u *)"global";
	    apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE,
								curbuf);
	}
	retval = TRUE;
    }
    vim_free(tofree);

    return retval;
}