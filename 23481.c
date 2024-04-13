asmlinkage long sys_setfsgid(gid_t gid)
{
	int old_fsgid;

	old_fsgid = current->fsgid;
	if (security_task_setgid(gid, (gid_t)-1, (gid_t)-1, LSM_SETID_FS))
		return old_fsgid;

	if (gid == current->gid || gid == current->egid ||
	    gid == current->sgid || gid == current->fsgid || 
	    capable(CAP_SETGID)) {
		if (gid != old_fsgid) {
			current->mm->dumpable = suid_dumpable;
			smp_wmb();
		}
		current->fsgid = gid;
		key_fsgid_changed(current);
		proc_id_connector(current, PROC_EVENT_GID);
	}
	return old_fsgid;
}