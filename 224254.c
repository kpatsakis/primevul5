void task_clear_jobctl_trapping(struct task_struct *task)
{
	if (unlikely(task->jobctl & JOBCTL_TRAPPING)) {
		task->jobctl &= ~JOBCTL_TRAPPING;
		smp_mb();	/* advised by wake_up_bit() */
		wake_up_bit(&task->jobctl, JOBCTL_TRAPPING_BIT);
	}
}