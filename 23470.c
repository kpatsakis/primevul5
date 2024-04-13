asmlinkage long sys_times(struct tms __user * tbuf)
{
	/*
	 *	In the SMP world we might just be unlucky and have one of
	 *	the times increment as we use it. Since the value is an
	 *	atomically safe type this is just fine. Conceptually its
	 *	as if the syscall took an instant longer to occur.
	 */
	if (tbuf) {
		struct tms tmp;
		struct task_struct *tsk = current;
		struct task_struct *t;
		cputime_t utime, stime, cutime, cstime;

		spin_lock_irq(&tsk->sighand->siglock);
		utime = tsk->signal->utime;
		stime = tsk->signal->stime;
		t = tsk;
		do {
			utime = cputime_add(utime, t->utime);
			stime = cputime_add(stime, t->stime);
			t = next_thread(t);
		} while (t != tsk);

		cutime = tsk->signal->cutime;
		cstime = tsk->signal->cstime;
		spin_unlock_irq(&tsk->sighand->siglock);

		tmp.tms_utime = cputime_to_clock_t(utime);
		tmp.tms_stime = cputime_to_clock_t(stime);
		tmp.tms_cutime = cputime_to_clock_t(cutime);
		tmp.tms_cstime = cputime_to_clock_t(cstime);
		if (copy_to_user(tbuf, &tmp, sizeof(struct tms)))
			return -EFAULT;
	}
	return (long) jiffies_64_to_clock_t(get_jiffies_64());
}