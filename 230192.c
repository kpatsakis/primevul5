static void nfs4_layoutreturn_done(struct rpc_task *task, void *calldata)
{
	struct nfs4_layoutreturn *lrp = calldata;
	struct nfs_server *server;

	dprintk("--> %s\n", __func__);

	if (!nfs41_sequence_process(task, &lrp->res.seq_res))
		return;

	/*
	 * Was there an RPC level error? Assume the call succeeded,
	 * and that we need to release the layout
	 */
	if (task->tk_rpc_status != 0 && RPC_WAS_SENT(task)) {
		lrp->res.lrs_present = 0;
		return;
	}

	server = NFS_SERVER(lrp->args.inode);
	switch (task->tk_status) {
	case -NFS4ERR_OLD_STATEID:
		if (nfs4_layout_refresh_old_stateid(&lrp->args.stateid,
					&lrp->args.range,
					lrp->args.inode))
			goto out_restart;
		/* Fallthrough */
	default:
		task->tk_status = 0;
		/* Fallthrough */
	case 0:
		break;
	case -NFS4ERR_DELAY:
		if (nfs4_async_handle_error(task, server, NULL, NULL) != -EAGAIN)
			break;
		goto out_restart;
	}
	dprintk("<-- %s\n", __func__);
	return;
out_restart:
	task->tk_status = 0;
	nfs4_sequence_free_slot(&lrp->res.seq_res);
	rpc_restart_call_prepare(task);
}