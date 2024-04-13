static void nfs41_free_stateid_done(struct rpc_task *task, void *calldata)
{
	struct nfs_free_stateid_data *data = calldata;

	nfs41_sequence_done(task, &data->res.seq_res);

	switch (task->tk_status) {
	case -NFS4ERR_DELAY:
		if (nfs4_async_handle_error(task, data->server, NULL, NULL) == -EAGAIN)
			rpc_restart_call_prepare(task);
	}
}