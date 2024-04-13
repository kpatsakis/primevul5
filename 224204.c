static int __copy_siginfo_from_user(int signo, kernel_siginfo_t *to,
				    const siginfo_t __user *from)
{
	if (copy_from_user(to, from, sizeof(struct kernel_siginfo)))
		return -EFAULT;
	to->si_signo = signo;
	return post_copy_siginfo_from_user(to, from);
}