au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    int		group = AUGROUP_ALL;

    for (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    if (p > arg)
    {
	group_name = vim_strnsave(arg, (int)(p - arg));
	if (group_name == NULL)		/* out of memory */
	    return AUGROUP_ERROR;
	group = au_find_group(group_name);
	if (group == AUGROUP_ERROR)
	    group = AUGROUP_ALL;	/* no match, use all groups */
	else
	    *argp = skipwhite(p);	/* match, skip over group name */
	vim_free(group_name);
    }
    return group;
}