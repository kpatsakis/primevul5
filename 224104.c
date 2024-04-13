static int copy_siginfo_from_user_any(kernel_siginfo_t *kinfo, siginfo_t *info)
{
#ifdef CONFIG_COMPAT
	/*
	 * Avoid hooking up compat syscalls and instead handle necessary
	 * conversions here. Note, this is a stop-gap measure and should not be
	 * considered a generic solution.
	 */
	if (in_compat_syscall())
		return copy_siginfo_from_user32(
			kinfo, (struct compat_siginfo __user *)info);
#endif
	return copy_siginfo_from_user(kinfo, info);
}