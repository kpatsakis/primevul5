int set_user_sigmask(const sigset_t __user *umask, size_t sigsetsize)
{
	sigset_t kmask;

	if (!umask)
		return 0;
	if (sigsetsize != sizeof(sigset_t))
		return -EINVAL;
	if (copy_from_user(&kmask, umask, sizeof(sigset_t)))
		return -EFAULT;

	set_restore_sigmask();
	current->saved_sigmask = current->blocked;
	set_current_blocked(&kmask);

	return 0;
}