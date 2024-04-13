asmlinkage long sys_setgid(gid_t gid)
{
	int old_egid = current->egid;
	int retval;

	retval = security_task_setgid(gid, (gid_t)-1, (gid_t)-1, LSM_SETID_ID);
	if (retval)
		return retval;

	if (capable(CAP_SETGID)) {
		if (old_egid != gid) {
			current->mm->dumpable = suid_dumpable;
			smp_wmb();
		}
		current->gid = current->egid = current->sgid = current->fsgid = gid;
	} else if ((gid == current->gid) || (gid == current->sgid)) {
		if (old_egid != gid) {
			current->mm->dumpable = suid_dumpable;
			smp_wmb();
		}
		current->egid = current->fsgid = gid;
	}
	else
		return -EPERM;

	key_fsgid_changed(current);
	proc_id_connector(current, PROC_EVENT_GID);
	return 0;
}