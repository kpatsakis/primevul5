static void nfs4_lock_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs4_lockdata *data = calldata;
	struct nfs4_state *state = data->lsp->ls_state;

	dprintk("%s: begin!\n", __func__);
	if (nfs_wait_on_sequence(data->arg.lock_seqid, task) != 0)
		goto out_wait;
	/* Do we need to do an open_to_lock_owner? */
	if (!test_bit(NFS_LOCK_INITIALIZED, &data->lsp->ls_flags)) {
		if (nfs_wait_on_sequence(data->arg.open_seqid, task) != 0) {
			goto out_release_lock_seqid;
		}
		nfs4_stateid_copy(&data->arg.open_stateid,
				&state->open_stateid);
		data->arg.new_lock_owner = 1;
		data->res.open_seqid = data->arg.open_seqid;
	} else {
		data->arg.new_lock_owner = 0;
		nfs4_stateid_copy(&data->arg.lock_stateid,
				&data->lsp->ls_stateid);
	}
	if (!nfs4_valid_open_stateid(state)) {
		data->rpc_status = -EBADF;
		task->tk_action = NULL;
		goto out_release_open_seqid;
	}
	data->timestamp = jiffies;
	if (nfs4_setup_sequence(data->server->nfs_client,
				&data->arg.seq_args,
				&data->res.seq_res,
				task) == 0)
		return;
out_release_open_seqid:
	nfs_release_seqid(data->arg.open_seqid);
out_release_lock_seqid:
	nfs_release_seqid(data->arg.lock_seqid);
out_wait:
	nfs4_sequence_done(task, &data->res.seq_res);
	dprintk("%s: done!, ret = %d\n", __func__, data->rpc_status);
}