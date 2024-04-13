do_sigaltstack (const stack_t *ss, stack_t *oss, unsigned long sp,
		size_t min_ss_size)
{
	struct task_struct *t = current;

	if (oss) {
		memset(oss, 0, sizeof(stack_t));
		oss->ss_sp = (void __user *) t->sas_ss_sp;
		oss->ss_size = t->sas_ss_size;
		oss->ss_flags = sas_ss_flags(sp) |
			(current->sas_ss_flags & SS_FLAG_BITS);
	}

	if (ss) {
		void __user *ss_sp = ss->ss_sp;
		size_t ss_size = ss->ss_size;
		unsigned ss_flags = ss->ss_flags;
		int ss_mode;

		if (unlikely(on_sig_stack(sp)))
			return -EPERM;

		ss_mode = ss_flags & ~SS_FLAG_BITS;
		if (unlikely(ss_mode != SS_DISABLE && ss_mode != SS_ONSTACK &&
				ss_mode != 0))
			return -EINVAL;

		if (ss_mode == SS_DISABLE) {
			ss_size = 0;
			ss_sp = NULL;
		} else {
			if (unlikely(ss_size < min_ss_size))
				return -ENOMEM;
		}

		t->sas_ss_sp = (unsigned long) ss_sp;
		t->sas_ss_size = ss_size;
		t->sas_ss_flags = ss_flags;
	}
	return 0;
}