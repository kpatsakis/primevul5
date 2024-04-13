static int set_one_prio(struct task_struct *p, int niceval, int error)
{
	int no_nice;

	if (p->uid != current->euid &&
		p->euid != current->euid && !capable(CAP_SYS_NICE)) {
		error = -EPERM;
		goto out;
	}
	if (niceval < task_nice(p) && !can_nice(p, niceval)) {
		error = -EACCES;
		goto out;
	}
	no_nice = security_task_setnice(p, niceval);
	if (no_nice) {
		error = no_nice;
		goto out;
	}
	if (error == -ESRCH)
		error = 0;
	set_user_nice(p, niceval);
out:
	return error;
}