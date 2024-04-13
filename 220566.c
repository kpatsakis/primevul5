post_chdir(cdscope_T scope)
{
    if (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    if (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	if (globaldir == NULL && pdir != NULL)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	if (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    if (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    else
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    else
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    shorten_fnames(TRUE);
}