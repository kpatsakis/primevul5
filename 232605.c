au_new_group(char_u *name)
{
    int		i;

    i = au_find_group(name);
    if (i == AUGROUP_ERROR)	/* the group doesn't exist yet, add it */
    {
	/* First try using a free entry. */
	for (i = 0; i < augroups.ga_len; ++i)
	    if (AUGROUP_NAME(i) == NULL)
		break;
	if (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	    return AUGROUP_ERROR;

	AUGROUP_NAME(i) = vim_strsave(name);
	if (AUGROUP_NAME(i) == NULL)
	    return AUGROUP_ERROR;
	if (i == augroups.ga_len)
	    ++augroups.ga_len;
    }

    return i;
}