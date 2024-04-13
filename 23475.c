asmlinkage long sys_setsid(void)
{
	struct task_struct *group_leader = current->group_leader;
	pid_t session;
	int err = -EPERM;

	write_lock_irq(&tasklist_lock);

	/* Fail if I am already a session leader */
	if (group_leader->signal->leader)
		goto out;

	session = group_leader->pid;
	/* Fail if a process group id already exists that equals the
	 * proposed session id.
	 *
	 * Don't check if session id == 1 because kernel threads use this
	 * session id and so the check will always fail and make it so
	 * init cannot successfully call setsid.
	 */
	if (session > 1 && find_task_by_pid_type(PIDTYPE_PGID, session))
		goto out;

	group_leader->signal->leader = 1;
	__set_special_pids(session, session);

	spin_lock(&group_leader->sighand->siglock);
	group_leader->signal->tty = NULL;
	spin_unlock(&group_leader->sighand->siglock);

	err = process_group(group_leader);
out:
	write_unlock_irq(&tasklist_lock);
	return err;
}