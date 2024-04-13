get_augroup_name(expand_T *xp UNUSED, int idx)
{
    if (idx == augroups.ga_len)		/* add "END" add the end */
	return (char_u *)"END";
    if (idx >= augroups.ga_len)		/* end of list */
	return NULL;
    if (AUGROUP_NAME(idx) == NULL || AUGROUP_NAME(idx) == get_deleted_augroup())
	/* skip deleted entries */
	return (char_u *)"";
    return AUGROUP_NAME(idx);		/* return a name */
}