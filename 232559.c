match_file_pat(
    char_u	*pattern,		/* pattern to match with */
    regprog_T	**prog,			/* pre-compiled regprog or NULL */
    char_u	*fname,			/* full path of file name */
    char_u	*sfname,		/* short file name or NULL */
    char_u	*tail,			/* tail of path */
    int		allow_dirs)		/* allow matching with dir */
{
    regmatch_T	regmatch;
    int		result = FALSE;

    regmatch.rm_ic = p_fic; /* ignore case if 'fileignorecase' is set */
    if (prog != NULL)
	regmatch.regprog = *prog;
    else
	regmatch.regprog = vim_regcomp(pattern, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    if (regmatch.regprog != NULL
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != NULL
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, tail, (colnr_T)0))))
	result = TRUE;

    if (prog != NULL)
	*prog = regmatch.regprog;
    else
	vim_regfree(regmatch.regprog);
    return result;
}