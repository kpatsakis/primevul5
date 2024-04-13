asmlinkage long sys_setpgid(pid_t pid, pid_t pgid)
{
	struct task_struct *p;
	struct task_struct *group_leader = current->group_leader;
	int err = -EINVAL;

	if (!pid)
		pid = group_leader->pid;
	if (!pgid)
		pgid = pid;
	if (pgid < 0)
		return -EINVAL;

	/* From this point forward we keep holding onto the tasklist lock
	 * so that our parent does not change from under us. -DaveM
	 */
	write_lock_irq(&tasklist_lock);

	err = -ESRCH;
	p = find_task_by_pid(pid);
	if (!p)
		goto out;

	err = -EINVAL;
	if (!thread_group_leader(p))
		goto out;

	if (p->real_parent == group_leader) {
		err = -EPERM;
		if (task_session(p) != task_session(group_leader))
			goto out;
		err = -EACCES;
		if (p->did_exec)
			goto out;
	} else {
		err = -ESRCH;
		if (p != group_leader)
			goto out;
	}

	err = -EPERM;
	if (p->signal->leader)
		goto out;

	if (pgid != pid) {
		struct task_struct *g =
			find_task_by_pid_type(PIDTYPE_PGID, pgid);

		if (!g || task_session(g) != task_session(group_leader))
			goto out;
	}

	err = security_task_setpgid(p, pgid);
	if (err)
		goto out;

	if (process_group(p) != pgid) {
		detach_pid(p, PIDTYPE_PGID);
		p->signal->pgrp = pgid;
		attach_pid(p, PIDTYPE_PGID, pgid);
	}

	err = 0;
out:
	/* All paths lead to here, thus we are safe. -DaveM */
	write_unlock_irq(&tasklist_lock);
	return err;
}