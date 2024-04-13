nfs4_bind_one_conn_to_session_done(struct rpc_task *task, void *calldata)
{
	struct nfs41_bind_conn_to_session_args *args = task->tk_msg.rpc_argp;
	struct nfs41_bind_conn_to_session_res *res = task->tk_msg.rpc_resp;
	struct nfs_client *clp = args->client;

	switch (task->tk_status) {
	case -NFS4ERR_BADSESSION:
	case -NFS4ERR_DEADSESSION:
		nfs4_schedule_session_recovery(clp->cl_session,
				task->tk_status);
	}
	if (args->dir == NFS4_CDFC4_FORE_OR_BOTH &&
			res->dir != NFS4_CDFS4_BOTH) {
		rpc_task_close_connection(task);
		if (args->retries++ < MAX_BIND_CONN_TO_SESSION_RETRIES)
			rpc_restart_call(task);
	}
}