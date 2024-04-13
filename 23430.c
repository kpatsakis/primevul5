asmlinkage long sys_getsid(pid_t pid)
{
	if (!pid)
		return process_session(current);
	else {
		int retval;
		struct task_struct *p;

		read_lock(&tasklist_lock);
		p = find_task_by_pid(pid);

		retval = -ESRCH;
		if (p) {
			retval = security_task_getsid(p);
			if (!retval)
				retval = process_session(p);
		}
		read_unlock(&tasklist_lock);
		return retval;
	}
}