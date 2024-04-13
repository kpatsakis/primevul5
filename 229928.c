static void nfs41_sequence_prepare(struct rpc_task *task, void *data)
{
	struct nfs4_sequence_data *calldata = data;
	struct nfs_client *clp = calldata->clp;
	struct nfs4_sequence_args *args;
	struct nfs4_sequence_res *res;

	args = task->tk_msg.rpc_argp;
	res = task->tk_msg.rpc_resp;

	nfs4_setup_sequence(clp, args, res, task);
}