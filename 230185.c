static void nfs4_layoutcommit_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs4_layoutcommit_data *data = calldata;
	struct nfs_server *server = NFS_SERVER(data->args.inode);

	nfs4_setup_sequence(server->nfs_client,
			&data->args.seq_args,
			&data->res.seq_res,
			task);
}