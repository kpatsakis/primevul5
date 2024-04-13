static void nfs41_free_stateid_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs_free_stateid_data *data = calldata;
	nfs4_setup_sequence(data->server->nfs_client,
			&data->args.seq_args,
			&data->res.seq_res,
			task);
}