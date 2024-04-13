static void do_notify_pidfd(struct task_struct *task)
{
	struct pid *pid;

	WARN_ON(task->exit_state == 0);
	pid = task_pid(task);
	wake_up_all(&pid->wait_pidfd);
}