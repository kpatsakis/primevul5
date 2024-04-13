static int nfs4_do_call_sync(struct rpc_clnt *clnt,
			     struct nfs_server *server,
			     struct rpc_message *msg,
			     struct nfs4_sequence_args *args,
			     struct nfs4_sequence_res *res,
			     unsigned short task_flags)
{
	struct nfs_client *clp = server->nfs_client;
	struct nfs4_call_sync_data data = {
		.seq_server = server,
		.seq_args = args,
		.seq_res = res,
	};
	struct rpc_task_setup task_setup = {
		.rpc_client = clnt,
		.rpc_message = msg,
		.callback_ops = clp->cl_mvops->call_sync_ops,
		.callback_data = &data,
		.flags = task_flags,
	};

	return nfs4_call_sync_custom(&task_setup);
}