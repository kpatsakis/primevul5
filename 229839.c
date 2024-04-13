static int _nfs4_proc_lookupp(struct inode *inode,
		struct nfs_fh *fhandle, struct nfs_fattr *fattr,
		struct nfs4_label *label)
{
	struct rpc_clnt *clnt = NFS_CLIENT(inode);
	struct nfs_server *server = NFS_SERVER(inode);
	int		       status;
	struct nfs4_lookupp_arg args = {
		.bitmask = server->attr_bitmask,
		.fh = NFS_FH(inode),
	};
	struct nfs4_lookupp_res res = {
		.server = server,
		.fattr = fattr,
		.label = label,
		.fh = fhandle,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LOOKUPP],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};

	args.bitmask = nfs4_bitmask(server, label);

	nfs_fattr_init(fattr);

	dprintk("NFS call  lookupp ino=0x%lx\n", inode->i_ino);
	status = nfs4_call_sync(clnt, server, &msg, &args.seq_args,
				&res.seq_res, 0);
	dprintk("NFS reply lookupp: %d\n", status);
	return status;
}