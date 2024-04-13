int in_group_p(gid_t grp)
{
	int retval = 1;
	if (grp != current->fsgid)
		retval = groups_search(current->group_info, grp);
	return retval;
}