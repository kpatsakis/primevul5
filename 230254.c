static int nfs41_proc_reclaim_complete(struct nfs_client *clp,
		const struct cred *cred)
{
	struct nfs4_reclaim_complete_data *calldata;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_RECLAIM_COMPLETE],
		.rpc_cred = cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = clp->cl_rpcclient,
		.rpc_message = &msg,
		.callback_ops = &nfs4_reclaim_complete_call_ops,
		.flags = RPC_TASK_NO_ROUND_ROBIN,
	};
	int status = -ENOMEM;

	dprintk("--> %s\n", __func__);
	calldata = kzalloc(sizeof(*calldata), GFP_NOFS);
	if (calldata == NULL)
		goto out;
	calldata->clp = clp;
	calldata->arg.one_fs = 0;

	nfs4_init_sequence(&calldata->arg.seq_args, &calldata->res.seq_res, 0, 1);
	msg.rpc_argp = &calldata->arg;
	msg.rpc_resp = &calldata->res;
	task_setup_data.callback_data = calldata;
	status = nfs4_call_sync_custom(&task_setup_data);
out:
	dprintk("<-- %s status=%d\n", __func__, status);
	return status;
}