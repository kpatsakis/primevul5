ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#if !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory
    if (*new_dir == NUL)
	ex_pwd(NULL);
    else
#endif
    {
	cdscope_T	scope = CDSCOPE_GLOBAL;

	if (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	    scope = CDSCOPE_WINDOW;
	else if (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	    scope = CDSCOPE_TABPAGE;

	if (changedir_func(new_dir, eap->forceit, scope))
	{
	    // Echo the new current directory if the command was typed.
	    if (KeyTyped || p_verbose >= 5)
		ex_pwd(eap);
	}
    }
}