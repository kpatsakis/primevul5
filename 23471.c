asmlinkage long sys_setuid(uid_t uid)
{
	int old_euid = current->euid;
	int old_ruid, old_suid, new_suid;
	int retval;

	retval = security_task_setuid(uid, (uid_t)-1, (uid_t)-1, LSM_SETID_ID);
	if (retval)
		return retval;

	old_ruid = current->uid;
	old_suid = current->suid;
	new_suid = old_suid;
	
	if (capable(CAP_SETUID)) {
		if (uid != old_ruid && set_user(uid, old_euid != uid) < 0)
			return -EAGAIN;
		new_suid = uid;
	} else if ((uid != current->uid) && (uid != new_suid))
		return -EPERM;

	if (old_euid != uid) {
		current->mm->dumpable = suid_dumpable;
		smp_wmb();
	}
	current->fsuid = current->euid = uid;
	current->suid = new_suid;

	key_fsuid_changed(current);
	proc_id_connector(current, PROC_EVENT_UID);

	return security_task_post_setuid(old_ruid, old_euid, old_suid, LSM_SETID_ID);
}