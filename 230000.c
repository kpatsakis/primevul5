static void nfs4_close_prepare(struct rpc_task *task, void *data)
{
	struct nfs4_closedata *calldata = data;
	struct nfs4_state *state = calldata->state;
	struct inode *inode = calldata->inode;
	struct pnfs_layout_hdr *lo;
	bool is_rdonly, is_wronly, is_rdwr;
	int call_close = 0;

	dprintk("%s: begin!\n", __func__);
	if (nfs_wait_on_sequence(calldata->arg.seqid, task) != 0)
		goto out_wait;

	task->tk_msg.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_OPEN_DOWNGRADE];
	spin_lock(&state->owner->so_lock);
	is_rdwr = test_bit(NFS_O_RDWR_STATE, &state->flags);
	is_rdonly = test_bit(NFS_O_RDONLY_STATE, &state->flags);
	is_wronly = test_bit(NFS_O_WRONLY_STATE, &state->flags);
	/* Calculate the change in open mode */
	calldata->arg.fmode = 0;
	if (state->n_rdwr == 0) {
		if (state->n_rdonly == 0)
			call_close |= is_rdonly;
		else if (is_rdonly)
			calldata->arg.fmode |= FMODE_READ;
		if (state->n_wronly == 0)
			call_close |= is_wronly;
		else if (is_wronly)
			calldata->arg.fmode |= FMODE_WRITE;
		if (calldata->arg.fmode != (FMODE_READ|FMODE_WRITE))
			call_close |= is_rdwr;
	} else if (is_rdwr)
		calldata->arg.fmode |= FMODE_READ|FMODE_WRITE;

	nfs4_sync_open_stateid(&calldata->arg.stateid, state);
	if (!nfs4_valid_open_stateid(state))
		call_close = 0;
	spin_unlock(&state->owner->so_lock);

	if (!call_close) {
		/* Note: exit _without_ calling nfs4_close_done */
		goto out_no_action;
	}

	if (!calldata->lr.roc && nfs4_wait_on_layoutreturn(inode, task)) {
		nfs_release_seqid(calldata->arg.seqid);
		goto out_wait;
	}

	lo = calldata->arg.lr_args ? calldata->arg.lr_args->layout : NULL;
	if (lo && !pnfs_layout_is_valid(lo)) {
		calldata->arg.lr_args = NULL;
		calldata->res.lr_res = NULL;
	}

	if (calldata->arg.fmode == 0)
		task->tk_msg.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_CLOSE];

	if (calldata->arg.fmode == 0 || calldata->arg.fmode == FMODE_READ) {
		/* Close-to-open cache consistency revalidation */
		if (!nfs4_have_delegation(inode, FMODE_READ))
			calldata->arg.bitmask = NFS_SERVER(inode)->cache_consistency_bitmask;
		else
			calldata->arg.bitmask = NULL;
	}

	calldata->arg.share_access =
		nfs4_map_atomic_open_share(NFS_SERVER(inode),
				calldata->arg.fmode, 0);

	if (calldata->res.fattr == NULL)
		calldata->arg.bitmask = NULL;
	else if (calldata->arg.bitmask == NULL)
		calldata->res.fattr = NULL;
	calldata->timestamp = jiffies;
	if (nfs4_setup_sequence(NFS_SERVER(inode)->nfs_client,
				&calldata->arg.seq_args,
				&calldata->res.seq_res,
				task) != 0)
		nfs_release_seqid(calldata->arg.seqid);
	dprintk("%s: done!\n", __func__);
	return;
out_no_action:
	task->tk_action = NULL;
out_wait:
	nfs4_sequence_done(task, &calldata->res.seq_res);
}