static inline char __user *si_expansion(const siginfo_t __user *info)
{
	return ((char __user *)info) + sizeof(struct kernel_siginfo);
}