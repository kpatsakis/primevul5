asmlinkage long sys_setfsuid(uid_t uid)
{
	int old_fsuid;

	old_fsuid = current->fsuid;
	if (security_task_setuid(uid, (uid_t)-1, (uid_t)-1, LSM_SETID_FS))
		return old_fsuid;

	if (uid == current->uid || uid == current->euid ||
	    uid == current->suid || uid == current->fsuid || 
	    capable(CAP_SETUID)) {
		if (uid != old_fsuid) {
			current->mm->dumpable = suid_dumpable;
			smp_wmb();
		}
		current->fsuid = uid;
	}

	key_fsuid_changed(current);
	proc_id_connector(current, PROC_EVENT_UID);

	security_task_post_setuid(old_fsuid, (uid_t)-1, (uid_t)-1, LSM_SETID_FS);

	return old_fsuid;
}