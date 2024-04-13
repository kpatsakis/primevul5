static int groups_from_user(struct group_info *group_info,
    gid_t __user *grouplist)
{
	int i;
	int count = group_info->ngroups;

	for (i = 0; i < group_info->nblocks; i++) {
		int cp_count = min(NGROUPS_PER_BLOCK, count);
		int off = i * NGROUPS_PER_BLOCK;
		int len = cp_count * sizeof(*grouplist);

		if (copy_from_user(group_info->blocks[i], grouplist+off, len))
			return -EFAULT;

		count -= cp_count;
	}
	return 0;
}