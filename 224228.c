int set_compat_user_sigmask(const compat_sigset_t __user *umask,
			    size_t sigsetsize)
{
	sigset_t kmask;

	if (!umask)
		return 0;
	if (sigsetsize != sizeof(compat_sigset_t))
		return -EINVAL;
	if (get_compat_sigset(&kmask, umask))
		return -EFAULT;

	set_restore_sigmask();
	current->saved_sigmask = current->blocked;
	set_current_blocked(&kmask);

	return 0;
}