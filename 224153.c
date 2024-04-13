COMPAT_SYSCALL_DEFINE1(sigpending, compat_old_sigset_t __user *, set32)
{
	sigset_t set;

	do_sigpending(&set);

	return put_user(set.sig[0], set32);
}