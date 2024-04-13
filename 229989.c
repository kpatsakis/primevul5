static struct rpc_task *_nfs41_proc_sequence(struct nfs_client *clp,
		const struct cred *cred,
		struct nfs4_slot *slot,
		bool is_privileged)
{
	struct nfs4_sequence_data *calldata;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_SEQUENCE],
		.rpc_cred = cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = clp->cl_rpcclient,
		.rpc_message = &msg,
		.callback_ops = &nfs41_sequence_ops,
		.flags = RPC_TASK_ASYNC | RPC_TASK_TIMEOUT,
	};
	struct rpc_task *ret;

	ret = ERR_PTR(-EIO);
	if (!refcount_inc_not_zero(&clp->cl_count))
		goto out_err;

	ret = ERR_PTR(-ENOMEM);
	calldata = kzalloc(sizeof(*calldata), GFP_NOFS);
	if (calldata == NULL)
		goto out_put_clp;
	nfs4_init_sequence(&calldata->args, &calldata->res, 0, is_privileged);
	nfs4_sequence_attach_slot(&calldata->args, &calldata->res, slot);
	msg.rpc_argp = &calldata->args;
	msg.rpc_resp = &calldata->res;
	calldata->clp = clp;
	task_setup_data.callback_data = calldata;

	ret = rpc_run_task(&task_setup_data);
	if (IS_ERR(ret))
		goto out_err;
	return ret;
out_put_clp:
	nfs_put_client(clp);
out_err:
	nfs41_release_slot(slot);
	return ret;
}