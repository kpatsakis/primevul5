SYSCALL_DEFINE1(sigpending, old_sigset_t __user *, uset)
{
	sigset_t set;

	if (sizeof(old_sigset_t) > sizeof(*uset))
		return -EINVAL;

	do_sigpending(&set);

	if (copy_to_user(uset, &set, sizeof(old_sigset_t)))
		return -EFAULT;

	return 0;
}