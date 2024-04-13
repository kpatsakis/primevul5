int copy_siginfo_to_user(siginfo_t __user *to, const kernel_siginfo_t *from)
{
	char __user *expansion = si_expansion(to);
	if (copy_to_user(to, from , sizeof(struct kernel_siginfo)))
		return -EFAULT;
	if (clear_user(expansion, SI_EXPANSION_SIZE))
		return -EFAULT;
	return 0;
}