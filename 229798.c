_nfs4_proc_remove(struct inode *dir, const struct qstr *name, u32 ftype)
{
	struct nfs_server *server = NFS_SERVER(dir);
	struct nfs_removeargs args = {
		.fh = NFS_FH(dir),
		.name = *name,
	};
	struct nfs_removeres res = {
		.server = server,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_REMOVE],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};
	unsigned long timestamp = jiffies;
	int status;

	status = nfs4_call_sync(server->client, server, &msg, &args.seq_args, &res.seq_res, 1);
	if (status == 0) {
		spin_lock(&dir->i_lock);
		nfs4_update_changeattr_locked(dir, &res.cinfo, timestamp,
					      NFS_INO_INVALID_DATA);
		/* Removing a directory decrements nlink in the parent */
		if (ftype == NF4DIR && dir->i_nlink > 2)
			nfs4_dec_nlink_locked(dir);
		spin_unlock(&dir->i_lock);
	}
	return status;
}