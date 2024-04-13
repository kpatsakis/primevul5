int nfs4_proc_bind_conn_to_session(struct nfs_client *clp, const struct cred *cred)
{
	struct rpc_bind_conn_calldata data = {
		.clp = clp,
		.cred = cred,
	};
	return rpc_clnt_iterate_for_each_xprt(clp->cl_rpcclient,
			nfs4_proc_bind_conn_to_session_callback, &data);
}