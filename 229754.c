static void nfs4_lock_done(struct rpc_task *task, void *calldata)
{
	struct nfs4_lockdata *data = calldata;
	struct nfs4_lock_state *lsp = data->lsp;

	dprintk("%s: begin!\n", __func__);

	if (!nfs4_sequence_done(task, &data->res.seq_res))
		return;

	data->rpc_status = task->tk_status;
	switch (task->tk_status) {
	case 0:
		renew_lease(NFS_SERVER(d_inode(data->ctx->dentry)),
				data->timestamp);
		if (data->arg.new_lock && !data->cancelled) {
			data->fl.fl_flags &= ~(FL_SLEEP | FL_ACCESS);
			if (locks_lock_inode_wait(lsp->ls_state->inode, &data->fl) < 0)
				goto out_restart;
		}
		if (data->arg.new_lock_owner != 0) {
			nfs_confirm_seqid(&lsp->ls_seqid, 0);
			nfs4_stateid_copy(&lsp->ls_stateid, &data->res.stateid);
			set_bit(NFS_LOCK_INITIALIZED, &lsp->ls_flags);
		} else if (!nfs4_update_lock_stateid(lsp, &data->res.stateid))
			goto out_restart;
		break;
	case -NFS4ERR_BAD_STATEID:
	case -NFS4ERR_OLD_STATEID:
	case -NFS4ERR_STALE_STATEID:
	case -NFS4ERR_EXPIRED:
		if (data->arg.new_lock_owner != 0) {
			if (!nfs4_stateid_match(&data->arg.open_stateid,
						&lsp->ls_state->open_stateid))
				goto out_restart;
		} else if (!nfs4_stateid_match(&data->arg.lock_stateid,
						&lsp->ls_stateid))
				goto out_restart;
	}
out_done:
	dprintk("%s: done, ret = %d!\n", __func__, data->rpc_status);
	return;
out_restart:
	if (!data->cancelled)
		rpc_restart_call_prepare(task);
	goto out_done;
}