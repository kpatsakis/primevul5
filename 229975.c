static void nfs4_close_done(struct rpc_task *task, void *data)
{
	struct nfs4_closedata *calldata = data;
	struct nfs4_state *state = calldata->state;
	struct nfs_server *server = NFS_SERVER(calldata->inode);
	nfs4_stateid *res_stateid = NULL;
	struct nfs4_exception exception = {
		.state = state,
		.inode = calldata->inode,
		.stateid = &calldata->arg.stateid,
	};

	dprintk("%s: begin!\n", __func__);
	if (!nfs4_sequence_done(task, &calldata->res.seq_res))
		return;
	trace_nfs4_close(state, &calldata->arg, &calldata->res, task->tk_status);

	/* Handle Layoutreturn errors */
	if (pnfs_roc_done(task, calldata->inode,
				&calldata->arg.lr_args,
				&calldata->res.lr_res,
				&calldata->res.lr_ret) == -EAGAIN)
		goto out_restart;

	/* hmm. we are done with the inode, and in the process of freeing
	 * the state_owner. we keep this around to process errors
	 */
	switch (task->tk_status) {
		case 0:
			res_stateid = &calldata->res.stateid;
			renew_lease(server, calldata->timestamp);
			break;
		case -NFS4ERR_ACCESS:
			if (calldata->arg.bitmask != NULL) {
				calldata->arg.bitmask = NULL;
				calldata->res.fattr = NULL;
				goto out_restart;

			}
			break;
		case -NFS4ERR_OLD_STATEID:
			/* Did we race with OPEN? */
			if (nfs4_refresh_open_old_stateid(&calldata->arg.stateid,
						state))
				goto out_restart;
			goto out_release;
		case -NFS4ERR_ADMIN_REVOKED:
		case -NFS4ERR_STALE_STATEID:
		case -NFS4ERR_EXPIRED:
			nfs4_free_revoked_stateid(server,
					&calldata->arg.stateid,
					task->tk_msg.rpc_cred);
			/* Fallthrough */
		case -NFS4ERR_BAD_STATEID:
			if (calldata->arg.fmode == 0)
				break;
			/* Fallthrough */
		default:
			task->tk_status = nfs4_async_handle_exception(task,
					server, task->tk_status, &exception);
			if (exception.retry)
				goto out_restart;
	}
	nfs_clear_open_stateid(state, &calldata->arg.stateid,
			res_stateid, calldata->arg.fmode);
out_release:
	task->tk_status = 0;
	nfs_release_seqid(calldata->arg.seqid);
	nfs_refresh_inode(calldata->inode, &calldata->fattr);
	dprintk("%s: done, ret = %d!\n", __func__, task->tk_status);
	return;
out_restart:
	task->tk_status = 0;
	rpc_restart_call_prepare(task);
	goto out_release;
}