COMPAT_SYSCALL_DEFINE2(rt_sigpending, compat_sigset_t __user *, uset,
		compat_size_t, sigsetsize)
{
	sigset_t set;

	if (sigsetsize > sizeof(*uset))
		return -EINVAL;

	do_sigpending(&set);

	return put_compat_sigset(uset, &set, sigsetsize);
}