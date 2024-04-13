set_file_options(int set_options, exarg_T *eap)
{
    /* set default 'fileformat' */
    if (set_options)
    {
	if (eap != NULL && eap->force_ff != 0)
	    set_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	else if (*p_ffs != NUL)
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    /* set or reset 'binary' */
    if (eap != NULL && eap->force_bin != 0)
    {
	int	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	set_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}