static int _nfs4_proc_commit(struct file *dst, struct nfs_commitargs *args,
				struct nfs_commitres *res)
{
	struct inode *dst_inode = file_inode(dst);
	struct nfs_server *server = NFS_SERVER(dst_inode);
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_COMMIT],
		.rpc_argp = args,
		.rpc_resp = res,
	};

	args->fh = NFS_FH(dst_inode);
	return nfs4_call_sync(server->client, server, &msg,
			&args->seq_args, &res->seq_res, 1);
}