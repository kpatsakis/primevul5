static int nfs41_lock_expired(struct nfs4_state *state, struct file_lock *request)
{
	struct nfs4_lock_state *lsp;
	int status;

	status = nfs4_set_lock_state(state, request);
	if (status != 0)
		return status;
	lsp = request->fl_u.nfs4_fl.owner;
	if (test_bit(NFS_LOCK_INITIALIZED, &lsp->ls_flags) ||
	    test_bit(NFS_LOCK_LOST, &lsp->ls_flags))
		return 0;
	return nfs4_lock_expired(state, request);
}