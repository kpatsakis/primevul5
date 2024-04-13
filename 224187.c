static int do_compat_sigaltstack(const compat_stack_t __user *uss_ptr,
				 compat_stack_t __user *uoss_ptr)
{
	stack_t uss, uoss;
	int ret;

	if (uss_ptr) {
		compat_stack_t uss32;
		if (copy_from_user(&uss32, uss_ptr, sizeof(compat_stack_t)))
			return -EFAULT;
		uss.ss_sp = compat_ptr(uss32.ss_sp);
		uss.ss_flags = uss32.ss_flags;
		uss.ss_size = uss32.ss_size;
	}
	ret = do_sigaltstack(uss_ptr ? &uss : NULL, &uoss,
			     compat_user_stack_pointer(),
			     COMPAT_MINSIGSTKSZ);
	if (ret >= 0 && uoss_ptr)  {
		compat_stack_t old;
		memset(&old, 0, sizeof(old));
		old.ss_sp = ptr_to_compat(uoss.ss_sp);
		old.ss_flags = uoss.ss_flags;
		old.ss_size = uoss.ss_size;
		if (copy_to_user(uoss_ptr, &old, sizeof(compat_stack_t)))
			ret = -EFAULT;
	}
	return ret;
}