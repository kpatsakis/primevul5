static int nfs4_run_open_task(struct nfs4_opendata *data,
			      struct nfs_open_context *ctx)
{
	struct inode *dir = d_inode(data->dir);
	struct nfs_server *server = NFS_SERVER(dir);
	struct nfs_openargs *o_arg = &data->o_arg;
	struct nfs_openres *o_res = &data->o_res;
	struct rpc_task *task;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_OPEN],
		.rpc_argp = o_arg,
		.rpc_resp = o_res,
		.rpc_cred = data->owner->so_cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = server->client,
		.rpc_message = &msg,
		.callback_ops = &nfs4_open_ops,
		.callback_data = data,
		.workqueue = nfsiod_workqueue,
		.flags = RPC_TASK_ASYNC | RPC_TASK_CRED_NOREF,
	};
	int status;

	kref_get(&data->kref);
	data->rpc_done = false;
	data->rpc_status = 0;
	data->cancelled = false;
	data->is_recover = false;
	if (!ctx) {
		nfs4_init_sequence(&o_arg->seq_args, &o_res->seq_res, 1, 1);
		data->is_recover = true;
		task_setup_data.flags |= RPC_TASK_TIMEOUT;
	} else {
		nfs4_init_sequence(&o_arg->seq_args, &o_res->seq_res, 1, 0);
		pnfs_lgopen_prepare(data, ctx);
	}
	task = rpc_run_task(&task_setup_data);
	if (IS_ERR(task))
		return PTR_ERR(task);
	status = rpc_wait_for_completion_task(task);
	if (status != 0) {
		data->cancelled = true;
		smp_wmb();
	} else
		status = data->rpc_status;
	rpc_put_task(task);

	return status;
}