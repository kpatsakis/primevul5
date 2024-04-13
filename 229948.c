static void nfs41_call_sync_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs4_call_sync_data *data = calldata;

	dprintk("--> %s data->seq_server %p\n", __func__, data->seq_server);

	nfs4_setup_sequence(data->seq_server->nfs_client,
			    data->seq_args, data->seq_res, task);
}