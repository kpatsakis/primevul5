int nfs4_proc_setclientid_confirm(struct nfs_client *clp,
		struct nfs4_setclientid_res *arg,
		const struct cred *cred)
{
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_SETCLIENTID_CONFIRM],
		.rpc_argp = arg,
		.rpc_cred = cred,
	};
	int status;

	dprintk("NFS call  setclientid_confirm auth=%s, (client ID %llx)\n",
		clp->cl_rpcclient->cl_auth->au_ops->au_name,
		clp->cl_clientid);
	status = rpc_call_sync(clp->cl_rpcclient, &msg,
			       RPC_TASK_TIMEOUT | RPC_TASK_NO_ROUND_ROBIN);
	trace_nfs4_setclientid_confirm(clp, status);
	dprintk("NFS reply setclientid_confirm: %d\n", status);
	return status;
}