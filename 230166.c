nfs4_proc_bind_conn_to_session_callback(struct rpc_clnt *clnt,
		struct rpc_xprt *xprt,
		void *calldata)
{
	struct rpc_bind_conn_calldata *p = calldata;

	return nfs4_proc_bind_one_conn_to_session(clnt, xprt, p->clp, p->cred);
}