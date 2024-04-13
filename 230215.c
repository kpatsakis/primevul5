nfs4_proc_layoutcommit(struct nfs4_layoutcommit_data *data, bool sync)
{
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LAYOUTCOMMIT],
		.rpc_argp = &data->args,
		.rpc_resp = &data->res,
		.rpc_cred = data->cred,
	};
	struct rpc_task_setup task_setup_data = {
		.task = &data->task,
		.rpc_client = NFS_CLIENT(data->args.inode),
		.rpc_message = &msg,
		.callback_ops = &nfs4_layoutcommit_ops,
		.callback_data = data,
	};
	struct rpc_task *task;
	int status = 0;

	dprintk("NFS: initiating layoutcommit call. sync %d "
		"lbw: %llu inode %lu\n", sync,
		data->args.lastbytewritten,
		data->args.inode->i_ino);

	if (!sync) {
		data->inode = nfs_igrab_and_active(data->args.inode);
		if (data->inode == NULL) {
			nfs4_layoutcommit_release(data);
			return -EAGAIN;
		}
		task_setup_data.flags = RPC_TASK_ASYNC;
	}
	nfs4_init_sequence(&data->args.seq_args, &data->res.seq_res, 1, 0);
	task = rpc_run_task(&task_setup_data);
	if (IS_ERR(task))
		return PTR_ERR(task);
	if (sync)
		status = task->tk_status;
	trace_nfs4_layoutcommit(data->args.inode, &data->args.stateid, status);
	dprintk("%s: status %d\n", __func__, status);
	rpc_put_task(task);
	return status;
}