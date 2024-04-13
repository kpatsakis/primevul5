void nfs4_test_session_trunk(struct rpc_clnt *clnt, struct rpc_xprt *xprt,
			    void *data)
{
	struct nfs4_add_xprt_data *adata = (struct nfs4_add_xprt_data *)data;
	struct rpc_task *task;
	int status;

	u32 sp4_how;

	dprintk("--> %s try %s\n", __func__,
		xprt->address_strings[RPC_DISPLAY_ADDR]);

	sp4_how = (adata->clp->cl_sp4_flags == 0 ? SP4_NONE : SP4_MACH_CRED);

	/* Test connection for session trunking. Async exchange_id call */
	task = nfs4_run_exchange_id(adata->clp, adata->cred, sp4_how, xprt);
	if (IS_ERR(task))
		return;

	status = task->tk_status;
	if (status == 0)
		status = nfs4_detect_session_trunking(adata->clp,
				task->tk_msg.rpc_resp, xprt);

	if (status == 0)
		rpc_clnt_xprt_switch_add_xprt(clnt, xprt);

	rpc_put_task(task);
}