static void nfs4_delegreturn_done(struct rpc_task *task, void *calldata)
{
	struct nfs4_delegreturndata *data = calldata;
	struct nfs4_exception exception = {
		.inode = data->inode,
		.stateid = &data->stateid,
	};

	if (!nfs4_sequence_done(task, &data->res.seq_res))
		return;

	trace_nfs4_delegreturn_exit(&data->args, &data->res, task->tk_status);

	/* Handle Layoutreturn errors */
	if (pnfs_roc_done(task, data->inode,
				&data->args.lr_args,
				&data->res.lr_res,
				&data->res.lr_ret) == -EAGAIN)
		goto out_restart;

	switch (task->tk_status) {
	case 0:
		renew_lease(data->res.server, data->timestamp);
		break;
	case -NFS4ERR_ADMIN_REVOKED:
	case -NFS4ERR_DELEG_REVOKED:
	case -NFS4ERR_EXPIRED:
		nfs4_free_revoked_stateid(data->res.server,
				data->args.stateid,
				task->tk_msg.rpc_cred);
		/* Fallthrough */
	case -NFS4ERR_BAD_STATEID:
	case -NFS4ERR_STALE_STATEID:
	case -ETIMEDOUT:
		task->tk_status = 0;
		break;
	case -NFS4ERR_OLD_STATEID:
		if (!nfs4_refresh_delegation_stateid(&data->stateid, data->inode))
			nfs4_stateid_seqid_inc(&data->stateid);
		if (data->args.bitmask) {
			data->args.bitmask = NULL;
			data->res.fattr = NULL;
		}
		goto out_restart;
	case -NFS4ERR_ACCESS:
		if (data->args.bitmask) {
			data->args.bitmask = NULL;
			data->res.fattr = NULL;
			goto out_restart;
		}
		/* Fallthrough */
	default:
		task->tk_status = nfs4_async_handle_exception(task,
				data->res.server, task->tk_status,
				&exception);
		if (exception.retry)
			goto out_restart;
	}
	nfs_delegation_mark_returned(data->inode, data->args.stateid);
	data->rpc_status = task->tk_status;
	return;
out_restart:
	task->tk_status = 0;
	rpc_restart_call_prepare(task);
}