static int nfs4_proc_getlk(struct nfs4_state *state, int cmd, struct file_lock *request)
{
	struct nfs4_exception exception = {
		.interruptible = true,
	};
	int err;

	do {
		err = _nfs4_proc_getlk(state, cmd, request);
		trace_nfs4_get_lock(request, state, cmd, err);
		err = nfs4_handle_exception(NFS_SERVER(state->inode), err,
				&exception);
	} while (exception.retry);
	return err;
}