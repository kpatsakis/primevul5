int in_egroup_p(gid_t grp)
{
	int retval = 1;
	if (grp != current->egid)
		retval = groups_search(current->group_info, grp);
	return retval;
}