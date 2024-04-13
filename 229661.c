int nfs4_proc_bind_one_conn_to_session(struct rpc_clnt *clnt,
		struct rpc_xprt *xprt,
		struct nfs_client *clp,
		const struct cred *cred)
{
	int status;
	struct nfs41_bind_conn_to_session_args args = {
		.client = clp,
		.dir = NFS4_CDFC4_FORE_OR_BOTH,
		.retries = 0,
	};
	struct nfs41_bind_conn_to_session_res res;
	struct rpc_message msg = {
		.rpc_proc =
			&nfs4_procedures[NFSPROC4_CLNT_BIND_CONN_TO_SESSION],
		.rpc_argp = &args,
		.rpc_resp = &res,
		.rpc_cred = cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = clnt,
		.rpc_xprt = xprt,
		.callback_ops = &nfs4_bind_one_conn_to_session_ops,
		.rpc_message = &msg,
		.flags = RPC_TASK_TIMEOUT,
	};
	struct rpc_task *task;

	nfs4_copy_sessionid(&args.sessionid, &clp->cl_session->sess_id);
	if (!(clp->cl_session->flags & SESSION4_BACK_CHAN))
		args.dir = NFS4_CDFC4_FORE;

	/* Do not set the backchannel flag unless this is clnt->cl_xprt */
	if (xprt != rcu_access_pointer(clnt->cl_xprt))
		args.dir = NFS4_CDFC4_FORE;

	task = rpc_run_task(&task_setup_data);
	if (!IS_ERR(task)) {
		status = task->tk_status;
		rpc_put_task(task);
	} else
		status = PTR_ERR(task);
	trace_nfs4_bind_conn_to_session(clp, status);
	if (status == 0) {
		if (memcmp(res.sessionid.data,
		    clp->cl_session->sess_id.data, NFS4_MAX_SESSIONID_LEN)) {
			dprintk("NFS: %s: Session ID mismatch\n", __func__);
			return -EIO;
		}
		if ((res.dir & args.dir) != res.dir || res.dir == 0) {
			dprintk("NFS: %s: Unexpected direction from server\n",
				__func__);
			return -EIO;
		}
		if (res.use_conn_in_rdma_mode != args.use_conn_in_rdma_mode) {
			dprintk("NFS: %s: Server returned RDMA mode = true\n",
				__func__);
			return -EIO;
		}
	}

	return status;
}