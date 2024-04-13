COMPAT_SYSCALL_DEFINE2(sigaltstack,
			const compat_stack_t __user *, uss_ptr,
			compat_stack_t __user *, uoss_ptr)
{
	return do_compat_sigaltstack(uss_ptr, uoss_ptr);
}