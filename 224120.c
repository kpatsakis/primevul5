void task_join_group_stop(struct task_struct *task)
{
	/* Have the new thread join an on-going signal group stop */
	unsigned long jobctl = current->jobctl;
	if (jobctl & JOBCTL_STOP_PENDING) {
		struct signal_struct *sig = current->signal;
		unsigned long signr = jobctl & JOBCTL_STOP_SIGMASK;
		unsigned long gstop = JOBCTL_STOP_PENDING | JOBCTL_STOP_CONSUME;
		if (task_set_jobctl_pending(task, signr | gstop)) {
			sig->group_stop_count++;
		}
	}
}