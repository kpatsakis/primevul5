int __save_altstack(stack_t __user *uss, unsigned long sp)
{
	struct task_struct *t = current;
	int err = __put_user((void __user *)t->sas_ss_sp, &uss->ss_sp) |
		__put_user(t->sas_ss_flags, &uss->ss_flags) |
		__put_user(t->sas_ss_size, &uss->ss_size);
	if (err)
		return err;
	if (t->sas_ss_flags & SS_AUTODISARM)
		sas_ss_reset(t);
	return 0;
}