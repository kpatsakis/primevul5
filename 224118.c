static int __copy_siginfo_from_user32(int signo, struct kernel_siginfo *to,
				      const struct compat_siginfo __user *ufrom)
{
	struct compat_siginfo from;

	if (copy_from_user(&from, ufrom, sizeof(struct compat_siginfo)))
		return -EFAULT;

	from.si_signo = signo;
	return post_copy_siginfo_from_user32(to, &from);
}