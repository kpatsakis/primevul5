asmlinkage long sys_setresuid(uid_t ruid, uid_t euid, uid_t suid)
{
	int old_ruid = current->uid;
	int old_euid = current->euid;
	int old_suid = current->suid;
	int retval;

	retval = security_task_setuid(ruid, euid, suid, LSM_SETID_RES);
	if (retval)
		return retval;

	if (!capable(CAP_SETUID)) {
		if ((ruid != (uid_t) -1) && (ruid != current->uid) &&
		    (ruid != current->euid) && (ruid != current->suid))
			return -EPERM;
		if ((euid != (uid_t) -1) && (euid != current->uid) &&
		    (euid != current->euid) && (euid != current->suid))
			return -EPERM;
		if ((suid != (uid_t) -1) && (suid != current->uid) &&
		    (suid != current->euid) && (suid != current->suid))
			return -EPERM;
	}
	if (ruid != (uid_t) -1) {
		if (ruid != current->uid && set_user(ruid, euid != current->euid) < 0)
			return -EAGAIN;
	}
	if (euid != (uid_t) -1) {
		if (euid != current->euid) {
			current->mm->dumpable = suid_dumpable;
			smp_wmb();
		}
		current->euid = euid;
	}
	current->fsuid = current->euid;
	if (suid != (uid_t) -1)
		current->suid = suid;

	key_fsuid_changed(current);
	proc_id_connector(current, PROC_EVENT_UID);

	return security_task_post_setuid(old_ruid, old_euid, old_suid, LSM_SETID_RES);
}