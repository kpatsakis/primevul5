nfs4_retry_setlk_simple(struct nfs4_state *state, int cmd,
			struct file_lock *request)
{
	int		status = -ERESTARTSYS;
	unsigned long	timeout = NFS4_LOCK_MINTIMEOUT;

	while(!signalled()) {
		status = nfs4_proc_setlk(state, cmd, request);
		if ((status != -EAGAIN) || IS_SETLK(cmd))
			break;
		freezable_schedule_timeout_interruptible(timeout);
		timeout *= 2;
		timeout = min_t(unsigned long, NFS4_LOCK_MAXTIMEOUT, timeout);
		status = -ERESTARTSYS;
	}
	return status;
}