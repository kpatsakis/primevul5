ex_mkspell(exarg_T *eap)
{
    int		fcount;
    char_u	**fnames;
    char_u	*arg = eap->arg;
    int		ascii = FALSE;

    if (STRNCMP(arg, "-ascii", 6) == 0)
    {
	ascii = TRUE;
	arg = skipwhite(arg + 6);
    }

    /* Expand all the remaining arguments (e.g., $VIMRUNTIME). */
    if (get_arglist_exp(arg, &fcount, &fnames, FALSE) == OK)
    {
	mkspell(fcount, fnames, ascii, eap->forceit, FALSE);
	FreeWild(fcount, fnames);
    }
}