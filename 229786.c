static int _nfs40_proc_fsid_present(struct inode *inode, const struct cred *cred)
{
	struct nfs_server *server = NFS_SERVER(inode);
	struct nfs_client *clp = NFS_SERVER(inode)->nfs_client;
	struct rpc_clnt *clnt = server->client;
	struct nfs4_fsid_present_arg args = {
		.fh		= NFS_FH(inode),
		.clientid	= clp->cl_clientid,
		.renew		= 1,		/* append RENEW */
	};
	struct nfs4_fsid_present_res res = {
		.renew		= 1,
	};
	struct rpc_message msg = {
		.rpc_proc	= &nfs4_procedures[NFSPROC4_CLNT_FSID_PRESENT],
		.rpc_argp	= &args,
		.rpc_resp	= &res,
		.rpc_cred	= cred,
	};
	unsigned long now = jiffies;
	int status;

	res.fh = nfs_alloc_fhandle();
	if (res.fh == NULL)
		return -ENOMEM;

	nfs4_init_sequence(&args.seq_args, &res.seq_res, 0, 1);
	status = nfs4_call_sync_sequence(clnt, server, &msg,
						&args.seq_args, &res.seq_res);
	nfs_free_fhandle(res.fh);
	if (status)
		return status;

	do_renew_lease(clp, now);
	return 0;
}