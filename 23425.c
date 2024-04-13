asmlinkage long sys_getpriority(int which, int who)
{
	struct task_struct *g, *p;
	struct user_struct *user;
	long niceval, retval = -ESRCH;
	struct pid *pgrp;

	if (which > 2 || which < 0)
		return -EINVAL;

	read_lock(&tasklist_lock);
	switch (which) {
		case PRIO_PROCESS:
			if (who)
				p = find_task_by_pid(who);
			else
				p = current;
			if (p) {
				niceval = 20 - task_nice(p);
				if (niceval > retval)
					retval = niceval;
			}
			break;
		case PRIO_PGRP:
			if (who)
				pgrp = find_pid(who);
			else
				pgrp = task_pgrp(current);
			do_each_pid_task(pgrp, PIDTYPE_PGID, p) {
				niceval = 20 - task_nice(p);
				if (niceval > retval)
					retval = niceval;
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
				if (p->uid == who) {
					niceval = 20 - task_nice(p);
					if (niceval > retval)
						retval = niceval;
				}
			while_each_thread(g, p);
			if (who != current->uid)
				free_uid(user);		/* for find_user() */
			break;
	}
out_unlock:
	read_unlock(&tasklist_lock);

	return retval;
}