static int post_copy_siginfo_from_user(kernel_siginfo_t *info,
				       const siginfo_t __user *from)
{
	if (unlikely(!known_siginfo_layout(info->si_signo, info->si_code))) {
		char __user *expansion = si_expansion(from);
		char buf[SI_EXPANSION_SIZE];
		int i;
		/*
		 * An unknown si_code might need more than
		 * sizeof(struct kernel_siginfo) bytes.  Verify all of the
		 * extra bytes are 0.  This guarantees copy_siginfo_to_user
		 * will return this data to userspace exactly.
		 */
		if (copy_from_user(&buf, expansion, SI_EXPANSION_SIZE))
			return -EFAULT;
		for (i = 0; i < SI_EXPANSION_SIZE; i++) {
			if (buf[i] != 0)
				return -E2BIG;
		}
	}
	return 0;
}