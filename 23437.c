static int set_user(uid_t new_ruid, int dumpclear)
{
	struct user_struct *new_user;

	new_user = alloc_uid(new_ruid);
	if (!new_user)
		return -EAGAIN;

	if (atomic_read(&new_user->processes) >=
				current->signal->rlim[RLIMIT_NPROC].rlim_cur &&
			new_user != &root_user) {
		free_uid(new_user);
		return -EAGAIN;
	}

	switch_uid(new_user);

	if (dumpclear) {
		current->mm->dumpable = suid_dumpable;
		smp_wmb();
	}
	current->uid = new_ruid;
	return 0;
}