static void nfs4_reclaim_complete_prepare(struct rpc_task *task, void *data)
{
	struct nfs4_reclaim_complete_data *calldata = data;

	nfs4_setup_sequence(calldata->clp,
			&calldata->arg.seq_args,
			&calldata->res.seq_res,
			task);
}