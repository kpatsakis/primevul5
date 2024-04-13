void finalize_exec(struct linux_binprm *bprm)
{
	/* Store any stack rlimit changes before starting thread. */
	task_lock(current->group_leader);
	current->signal->rlim[RLIMIT_STACK] = bprm->rlim_stack;
	task_unlock(current->group_leader);
}