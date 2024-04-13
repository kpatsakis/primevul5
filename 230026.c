static int _nfs4_proc_link(struct inode *inode, struct inode *dir, const struct qstr *name)
{
	struct nfs_server *server = NFS_SERVER(inode);
	__u32 bitmask[NFS4_BITMASK_SZ];
	struct nfs4_link_arg arg = {
		.fh     = NFS_FH(inode),
		.dir_fh = NFS_FH(dir),
		.name   = name,
		.bitmask = bitmask,
	};
	struct nfs4_link_res res = {
		.server = server,
		.label = NULL,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LINK],
		.rpc_argp = &arg,
		.rpc_resp = &res,
	};
	int status = -ENOMEM;

	res.fattr = nfs_alloc_fattr();
	if (res.fattr == NULL)
		goto out;

	res.label = nfs4_label_alloc(server, GFP_KERNEL);
	if (IS_ERR(res.label)) {
		status = PTR_ERR(res.label);
		goto out;
	}

	nfs4_inode_make_writeable(inode);
	nfs4_bitmap_copy_adjust_setattr(bitmask, nfs4_bitmask(server, res.label), inode);

	status = nfs4_call_sync(server->client, server, &msg, &arg.seq_args, &res.seq_res, 1);
	if (!status) {
		nfs4_update_changeattr(dir, &res.cinfo, res.fattr->time_start,
				       NFS_INO_INVALID_DATA);
		status = nfs_post_op_update_inode(inode, res.fattr);
		if (!status)
			nfs_setsecurity(inode, res.fattr, res.label);
	}


	nfs4_label_free(res.label);

out:
	nfs_free_fattr(res.fattr);
	return status;
}