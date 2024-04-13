get_event_name(expand_T *xp UNUSED, int idx)
{
    if (idx < augroups.ga_len)		/* First list group names, if wanted */
    {
	if (!include_groups || AUGROUP_NAME(idx) == NULL
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    return (char_u *)"";	/* skip deleted entries */
	return AUGROUP_NAME(idx);	/* return a name */
    }
    return (char_u *)event_names[idx - augroups.ga_len].name;
}