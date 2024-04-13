static void nfs4_proc_rename_rpc_prepare(struct rpc_task *task, struct nfs_renamedata *data)
{
	nfs4_setup_sequence(NFS_SERVER(data->old_dir)->nfs_client,
			&data->args.seq_args,
			&data->res.seq_res,
			task);
}