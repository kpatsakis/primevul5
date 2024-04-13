int restore_altstack(const stack_t __user *uss)
{
	stack_t new;
	if (copy_from_user(&new, uss, sizeof(stack_t)))
		return -EFAULT;
	(void)do_sigaltstack(&new, NULL, current_user_stack_pointer(),
			     MINSIGSTKSZ);
	/* squash all but EFAULT for now */
	return 0;
}