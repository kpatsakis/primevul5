asmlinkage long sys_move_pages(pid_t pid, unsigned long nr_pages,
			const void __user * __user *pages,
			const int __user *nodes,
			int __user *status, int flags)
{
	int err = 0;
	int i;
	struct task_struct *task;
	nodemask_t task_nodes;
	struct mm_struct *mm;
	struct page_to_node *pm = NULL;

	/* Check flags */
	if (flags & ~(MPOL_MF_MOVE|MPOL_MF_MOVE_ALL))
		return -EINVAL;

	if ((flags & MPOL_MF_MOVE_ALL) && !capable(CAP_SYS_NICE))
		return -EPERM;

	/* Find the mm_struct */
	read_lock(&tasklist_lock);
	task = pid ? find_task_by_vpid(pid) : current;
	if (!task) {
		read_unlock(&tasklist_lock);
		return -ESRCH;
	}
	mm = get_task_mm(task);
	read_unlock(&tasklist_lock);

	if (!mm)
		return -EINVAL;

	/*
	 * Check if this process has the right to modify the specified
	 * process. The right exists if the process has administrative
	 * capabilities, superuser privileges or the same
	 * userid as the target process.
	 */
	if ((current->euid != task->suid) && (current->euid != task->uid) &&
	    (current->uid != task->suid) && (current->uid != task->uid) &&
	    !capable(CAP_SYS_NICE)) {
		err = -EPERM;
		goto out2;
	}

 	err = security_task_movememory(task);
 	if (err)
 		goto out2;


	task_nodes = cpuset_mems_allowed(task);

	/* Limit nr_pages so that the multiplication may not overflow */
	if (nr_pages >= ULONG_MAX / sizeof(struct page_to_node) - 1) {
		err = -E2BIG;
		goto out2;
	}

	pm = vmalloc((nr_pages + 1) * sizeof(struct page_to_node));
	if (!pm) {
		err = -ENOMEM;
		goto out2;
	}

	/*
	 * Get parameters from user space and initialize the pm
	 * array. Return various errors if the user did something wrong.
	 */
	for (i = 0; i < nr_pages; i++) {
		const void __user *p;

		err = -EFAULT;
		if (get_user(p, pages + i))
			goto out;

		pm[i].addr = (unsigned long)p;
		if (nodes) {
			int node;

			if (get_user(node, nodes + i))
				goto out;

			err = -ENODEV;
			if (!node_state(node, N_HIGH_MEMORY))
				goto out;

			err = -EACCES;
			if (!node_isset(node, task_nodes))
				goto out;

			pm[i].node = node;
		} else
			pm[i].node = 0;	/* anything to not match MAX_NUMNODES */
	}
	/* End marker */
	pm[nr_pages].node = MAX_NUMNODES;

	if (nodes)
		err = do_move_pages(mm, pm, flags & MPOL_MF_MOVE_ALL);
	else
		err = do_pages_stat(mm, pm);

	if (err >= 0)
		/* Return status information */
		for (i = 0; i < nr_pages; i++)
			if (put_user(pm[i].status, status + i))
				err = -EFAULT;

out:
	vfree(pm);
out2:
	mmput(mm);
	return err;
}