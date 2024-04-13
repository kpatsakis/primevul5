int nfs4_proc_layoutreturn(struct nfs4_layoutreturn *lrp, bool sync)
{
	struct rpc_task *task;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LAYOUTRETURN],
		.rpc_argp = &lrp->args,
		.rpc_resp = &lrp->res,
		.rpc_cred = lrp->cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = NFS_SERVER(lrp->args.inode)->client,
		.rpc_message = &msg,
		.callback_ops = &nfs4_layoutreturn_call_ops,
		.callback_data = lrp,
	};
	int status = 0;

	nfs4_state_protect(NFS_SERVER(lrp->args.inode)->nfs_client,
			NFS_SP4_MACH_CRED_PNFS_CLEANUP,
			&task_setup_data.rpc_client, &msg);

	dprintk("--> %s\n", __func__);
	if (!sync) {
		lrp->inode = nfs_igrab_and_active(lrp->args.inode);
		if (!lrp->inode) {
			nfs4_layoutreturn_release(lrp);
			return -EAGAIN;
		}
		task_setup_data.flags |= RPC_TASK_ASYNC;
	}
	nfs4_init_sequence(&lrp->args.seq_args, &lrp->res.seq_res, 1, 0);
	task = rpc_run_task(&task_setup_data);
	if (IS_ERR(task))
		return PTR_ERR(task);
	if (sync)
		status = task->tk_status;
	trace_nfs4_layoutreturn(lrp->args.inode, &lrp->args.stateid, status);
	dprintk("<-- %s status=%d\n", __func__, status);
	rpc_put_task(task);
	return status;
}