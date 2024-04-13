asmlinkage long sys_setreuid(uid_t ruid, uid_t euid)
{
	int old_ruid, old_euid, old_suid, new_ruid, new_euid;
	int retval;

	retval = security_task_setuid(ruid, euid, (uid_t)-1, LSM_SETID_RE);
	if (retval)
		return retval;

	new_ruid = old_ruid = current->uid;
	new_euid = old_euid = current->euid;
	old_suid = current->suid;

	if (ruid != (uid_t) -1) {
		new_ruid = ruid;
		if ((old_ruid != ruid) &&
		    (current->euid != ruid) &&
		    !capable(CAP_SETUID))
			return -EPERM;
	}

	if (euid != (uid_t) -1) {
		new_euid = euid;
		if ((old_ruid != euid) &&
		    (current->euid != euid) &&
		    (current->suid != euid) &&
		    !capable(CAP_SETUID))
			return -EPERM;
	}

	if (new_ruid != old_ruid && set_user(new_ruid, new_euid != old_euid) < 0)
		return -EAGAIN;

	if (new_euid != old_euid) {
		current->mm->dumpable = suid_dumpable;
		smp_wmb();
	}
	current->fsuid = current->euid = new_euid;
	if (ruid != (uid_t) -1 ||
	    (euid != (uid_t) -1 && euid != old_ruid))
		current->suid = current->euid;
	current->fsuid = current->euid;

	key_fsuid_changed(current);
	proc_id_connector(current, PROC_EVENT_UID);

	return security_task_post_setuid(old_ruid, old_euid, old_suid, LSM_SETID_RE);
}