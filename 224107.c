SYSCALL_DEFINE2(rt_sigpending, sigset_t __user *, uset, size_t, sigsetsize)
{
	sigset_t set;

	if (sigsetsize > sizeof(*uset))
		return -EINVAL;

	do_sigpending(&set);

	if (copy_to_user(uset, &set, sigsetsize))
		return -EFAULT;

	return 0;
}