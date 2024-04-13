nfs4_run_exchange_id(struct nfs_client *clp, const struct cred *cred,
			u32 sp4_how, struct rpc_xprt *xprt)
{
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_EXCHANGE_ID],
		.rpc_cred = cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = clp->cl_rpcclient,
		.callback_ops = &nfs4_exchange_id_call_ops,
		.rpc_message = &msg,
		.flags = RPC_TASK_TIMEOUT | RPC_TASK_NO_ROUND_ROBIN,
	};
	struct nfs41_exchange_id_data *calldata;
	int status;

	if (!refcount_inc_not_zero(&clp->cl_count))
		return ERR_PTR(-EIO);

	status = -ENOMEM;
	calldata = kzalloc(sizeof(*calldata), GFP_NOFS);
	if (!calldata)
		goto out;

	nfs4_init_boot_verifier(clp, &calldata->args.verifier);

	status = nfs4_init_uniform_client_string(clp);
	if (status)
		goto out_calldata;

	calldata->res.server_owner = kzalloc(sizeof(struct nfs41_server_owner),
						GFP_NOFS);
	status = -ENOMEM;
	if (unlikely(calldata->res.server_owner == NULL))
		goto out_calldata;

	calldata->res.server_scope = kzalloc(sizeof(struct nfs41_server_scope),
					GFP_NOFS);
	if (unlikely(calldata->res.server_scope == NULL))
		goto out_server_owner;

	calldata->res.impl_id = kzalloc(sizeof(struct nfs41_impl_id), GFP_NOFS);
	if (unlikely(calldata->res.impl_id == NULL))
		goto out_server_scope;

	switch (sp4_how) {
	case SP4_NONE:
		calldata->args.state_protect.how = SP4_NONE;
		break;

	case SP4_MACH_CRED:
		calldata->args.state_protect = nfs4_sp4_mach_cred_request;
		break;

	default:
		/* unsupported! */
		WARN_ON_ONCE(1);
		status = -EINVAL;
		goto out_impl_id;
	}
	if (xprt) {
		task_setup_data.rpc_xprt = xprt;
		task_setup_data.flags |= RPC_TASK_SOFTCONN;
		memcpy(calldata->args.verifier.data, clp->cl_confirm.data,
				sizeof(calldata->args.verifier.data));
	}
	calldata->args.client = clp;
	calldata->args.flags = EXCHGID4_FLAG_SUPP_MOVED_REFER |
	EXCHGID4_FLAG_BIND_PRINC_STATEID;
#ifdef CONFIG_NFS_V4_1_MIGRATION
	calldata->args.flags |= EXCHGID4_FLAG_SUPP_MOVED_MIGR;
#endif
	msg.rpc_argp = &calldata->args;
	msg.rpc_resp = &calldata->res;
	task_setup_data.callback_data = calldata;

	return rpc_run_task(&task_setup_data);

out_impl_id:
	kfree(calldata->res.impl_id);
out_server_scope:
	kfree(calldata->res.server_scope);
out_server_owner:
	kfree(calldata->res.server_owner);
out_calldata:
	kfree(calldata);
out:
	nfs_put_client(clp);
	return ERR_PTR(status);
}