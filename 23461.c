int set_current_groups(struct group_info *group_info)
{
	int retval;
	struct group_info *old_info;

	retval = security_task_setgroups(group_info);
	if (retval)
		return retval;

	groups_sort(group_info);
	get_group_info(group_info);

	task_lock(current);
	old_info = current->group_info;
	current->group_info = group_info;
	task_unlock(current);

	put_group_info(old_info);

	return 0;
}