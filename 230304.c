static void nfs4_proc_unlink_rpc_prepare(struct rpc_task *task, struct nfs_unlinkdata *data)
{
	nfs4_setup_sequence(NFS_SB(data->dentry->d_sb)->nfs_client,
			&data->args.seq_args,
			&data->res.seq_res,
			task);
}