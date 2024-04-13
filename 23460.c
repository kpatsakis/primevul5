asmlinkage long sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
	int retval;

	retval = security_task_setgid(rgid, egid, sgid, LSM_SETID_RES);
	if (retval)
		return retval;

	if (!capable(CAP_SETGID)) {
		if ((rgid != (gid_t) -1) && (rgid != current->gid) &&
		    (rgid != current->egid) && (rgid != current->sgid))
			return -EPERM;
		if ((egid != (gid_t) -1) && (egid != current->gid) &&
		    (egid != current->egid) && (egid != current->sgid))
			return -EPERM;
		if ((sgid != (gid_t) -1) && (sgid != current->gid) &&
		    (sgid != current->egid) && (sgid != current->sgid))
			return -EPERM;
	}
	if (egid != (gid_t) -1) {
		if (egid != current->egid) {
			current->mm->dumpable = suid_dumpable;
			smp_wmb();
		}
		current->egid = egid;
	}
	current->fsgid = current->egid;
	if (rgid != (gid_t) -1)
		current->gid = rgid;
	if (sgid != (gid_t) -1)
		current->sgid = sgid;

	key_fsgid_changed(current);
	proc_id_connector(current, PROC_EVENT_GID);
	return 0;
}