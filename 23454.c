asmlinkage long sys_setpriority(int which, int who, int niceval)
{
	struct task_struct *g, *p;
	struct user_struct *user;
	int error = -EINVAL;
	struct pid *pgrp;

	if (which > 2 || which < 0)
		goto out;

	/* normalize: avoid signed division (rounding problems) */
	error = -ESRCH;
	if (niceval < -20)
		niceval = -20;
	if (niceval > 19)
		niceval = 19;

	read_lock(&tasklist_lock);
	switch (which) {
		case PRIO_PROCESS:
			if (who)
				p = find_task_by_pid(who);
			else
				p = current;
			if (p)
				error = set_one_prio(p, niceval, error);
			break;
		case PRIO_PGRP:
			if (who)
				pgrp = find_pid(who);
			else
				pgrp = task_pgrp(current);
			do_each_pid_task(pgrp, PIDTYPE_PGID, p) {
				error = set_one_prio(p, niceval, error);
			} while_each_pid_task(pgrp, PIDTYPE_PGID, p);
			break;
		case PRIO_USER:
			user = current->user;
			if (!who)
				who = current->uid;
			else
				if ((who != current->uid) && !(user = find_user(who)))
					goto out_unlock;	/* No processes for this user */

			do_each_thread(g, p)
				if (p->uid == who)
					error = set_one_prio(p, niceval, error);
			while_each_thread(g, p);
			if (who != current->uid)
				free_uid(user);		/* For find_user() */
			break;
	}
out_unlock:
	read_unlock(&tasklist_lock);
out:
	return error;
}