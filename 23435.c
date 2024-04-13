static int groups_to_user(gid_t __user *grouplist,
    struct group_info *group_info)
{
	int i;
	int count = group_info->ngroups;

	for (i = 0; i < group_info->nblocks; i++) {
		int cp_count = min(NGROUPS_PER_BLOCK, count);
		int off = i * NGROUPS_PER_BLOCK;
		int len = cp_count * sizeof(*grouplist);

		if (copy_to_user(grouplist+off, group_info->blocks[i], len))
			return -EFAULT;

		count -= cp_count;
	}
	return 0;
}