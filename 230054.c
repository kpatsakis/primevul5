static int nfs4_proc_unlck(struct nfs4_state *state, int cmd, struct file_lock *request)
{
	struct inode *inode = state->inode;
	struct nfs4_state_owner *sp = state->owner;
	struct nfs_inode *nfsi = NFS_I(inode);
	struct nfs_seqid *seqid;
	struct nfs4_lock_state *lsp;
	struct rpc_task *task;
	struct nfs_seqid *(*alloc_seqid)(struct nfs_seqid_counter *, gfp_t);
	int status = 0;
	unsigned char fl_flags = request->fl_flags;

	status = nfs4_set_lock_state(state, request);
	/* Unlock _before_ we do the RPC call */
	request->fl_flags |= FL_EXISTS;
	/* Exclude nfs_delegation_claim_locks() */
	mutex_lock(&sp->so_delegreturn_mutex);
	/* Exclude nfs4_reclaim_open_stateid() - note nesting! */
	down_read(&nfsi->rwsem);
	if (locks_lock_inode_wait(inode, request) == -ENOENT) {
		up_read(&nfsi->rwsem);
		mutex_unlock(&sp->so_delegreturn_mutex);
		goto out;
	}
	up_read(&nfsi->rwsem);
	mutex_unlock(&sp->so_delegreturn_mutex);
	if (status != 0)
		goto out;
	/* Is this a delegated lock? */
	lsp = request->fl_u.nfs4_fl.owner;
	if (test_bit(NFS_LOCK_INITIALIZED, &lsp->ls_flags) == 0)
		goto out;
	alloc_seqid = NFS_SERVER(inode)->nfs_client->cl_mvops->alloc_seqid;
	seqid = alloc_seqid(&lsp->ls_seqid, GFP_KERNEL);
	status = -ENOMEM;
	if (IS_ERR(seqid))
		goto out;
	task = nfs4_do_unlck(request, nfs_file_open_context(request->fl_file), lsp, seqid);
	status = PTR_ERR(task);
	if (IS_ERR(task))
		goto out;
	status = rpc_wait_for_completion_task(task);
	rpc_put_task(task);
out:
	request->fl_flags = fl_flags;
	trace_nfs4_unlock(request, state, F_SETLK, status);
	return status;
}