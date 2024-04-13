static struct task_struct * futex_find_get_task(pid_t pid)
{
	struct task_struct *p;
	const struct cred *cred = current_cred(), *pcred;

	rcu_read_lock();
	p = find_task_by_vpid(pid);
	if (!p) {
		p = ERR_PTR(-ESRCH);
	} else {
		pcred = __task_cred(p);
		if (cred->euid != pcred->euid &&
		    cred->euid != pcred->uid)
			p = ERR_PTR(-ESRCH);
		else
			get_task_struct(p);
	}

	rcu_read_unlock();

	return p;
}