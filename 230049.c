nfs4_proc_layoutget(struct nfs4_layoutget *lgp, long *timeout)
{
	struct inode *inode = lgp->args.inode;
	struct nfs_server *server = NFS_SERVER(inode);
	struct rpc_task *task;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LAYOUTGET],
		.rpc_argp = &lgp->args,
		.rpc_resp = &lgp->res,
		.rpc_cred = lgp->cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = server->client,
		.rpc_message = &msg,
		.callback_ops = &nfs4_layoutget_call_ops,
		.callback_data = lgp,
		.flags = RPC_TASK_ASYNC | RPC_TASK_CRED_NOREF,
	};
	struct pnfs_layout_segment *lseg = NULL;
	struct nfs4_exception exception = {
		.inode = inode,
		.timeout = *timeout,
	};
	int status = 0;

	dprintk("--> %s\n", __func__);

	/* nfs4_layoutget_release calls pnfs_put_layout_hdr */
	pnfs_get_layout_hdr(NFS_I(inode)->layout);

	nfs4_init_sequence(&lgp->args.seq_args, &lgp->res.seq_res, 0, 0);

	task = rpc_run_task(&task_setup_data);

	status = rpc_wait_for_completion_task(task);
	if (status != 0)
		goto out;

	if (task->tk_status < 0) {
		status = nfs4_layoutget_handle_exception(task, lgp, &exception);
		*timeout = exception.timeout;
	} else if (lgp->res.layoutp->len == 0) {
		status = -EAGAIN;
		*timeout = nfs4_update_delay(&exception.timeout);
	} else
		lseg = pnfs_layout_process(lgp);
out:
	trace_nfs4_layoutget(lgp->args.ctx,
			&lgp->args.range,
			&lgp->res.range,
			&lgp->res.stateid,
			status);

	rpc_put_task(task);
	dprintk("<-- %s status=%d\n", __func__, status);
	if (status)
		return ERR_PTR(status);
	return lseg;
}