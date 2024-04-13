asmlinkage long sys_setregid(gid_t rgid, gid_t egid)
{
	int old_rgid = current->gid;
	int old_egid = current->egid;
	int new_rgid = old_rgid;
	int new_egid = old_egid;
	int retval;

	retval = security_task_setgid(rgid, egid, (gid_t)-1, LSM_SETID_RE);
	if (retval)
		return retval;

	if (rgid != (gid_t) -1) {
		if ((old_rgid == rgid) ||
		    (current->egid==rgid) ||
		    capable(CAP_SETGID))
			new_rgid = rgid;
		else
			return -EPERM;
	}
	if (egid != (gid_t) -1) {
		if ((old_rgid == egid) ||
		    (current->egid == egid) ||
		    (current->sgid == egid) ||
		    capable(CAP_SETGID))
			new_egid = egid;
		else
			return -EPERM;
	}
	if (new_egid != old_egid) {
		current->mm->dumpable = suid_dumpable;
		smp_wmb();
	}
	if (rgid != (gid_t) -1 ||
	    (egid != (gid_t) -1 && egid != old_rgid))
		current->sgid = new_egid;
	current->fsgid = new_egid;
	current->egid = new_egid;
	current->gid = new_rgid;
	key_fsgid_changed(current);
	proc_id_connector(current, PROC_EVENT_GID);
	return 0;
}