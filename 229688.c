static void nfs4_proc_commit_rpc_prepare(struct rpc_task *task, struct nfs_commit_data *data)
{
	nfs4_setup_sequence(NFS_SERVER(data->inode)->nfs_client,
			&data->args.seq_args,
			&data->res.seq_res,
			task);
}