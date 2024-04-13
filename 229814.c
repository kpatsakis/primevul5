static int _nfs4_proc_getattr(struct nfs_server *server, struct nfs_fh *fhandle,
				struct nfs_fattr *fattr, struct nfs4_label *label,
				struct inode *inode)
{
	__u32 bitmask[NFS4_BITMASK_SZ];
	struct nfs4_getattr_arg args = {
		.fh = fhandle,
		.bitmask = bitmask,
	};
	struct nfs4_getattr_res res = {
		.fattr = fattr,
		.label = label,
		.server = server,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_GETATTR],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};
	unsigned short task_flags = 0;

	/* Is this is an attribute revalidation, subject to softreval? */
	if (inode && (server->flags & NFS_MOUNT_SOFTREVAL))
		task_flags |= RPC_TASK_TIMEOUT;

	nfs4_bitmap_copy_adjust(bitmask, nfs4_bitmask(server, label), inode);

	nfs_fattr_init(fattr);
	nfs4_init_sequence(&args.seq_args, &res.seq_res, 0, 0);
	return nfs4_do_call_sync(server->client, server, &msg,
			&args.seq_args, &res.seq_res, task_flags);
}