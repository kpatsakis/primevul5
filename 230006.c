static int _nfs4_do_set_security_label(struct inode *inode,
		struct nfs4_label *ilabel,
		struct nfs_fattr *fattr,
		struct nfs4_label *olabel)
{

	struct iattr sattr = {0};
	struct nfs_server *server = NFS_SERVER(inode);
	const u32 bitmask[3] = { 0, 0, FATTR4_WORD2_SECURITY_LABEL };
	struct nfs_setattrargs arg = {
		.fh		= NFS_FH(inode),
		.iap		= &sattr,
		.server		= server,
		.bitmask	= bitmask,
		.label		= ilabel,
	};
	struct nfs_setattrres res = {
		.fattr		= fattr,
		.label		= olabel,
		.server		= server,
	};
	struct rpc_message msg = {
		.rpc_proc	= &nfs4_procedures[NFSPROC4_CLNT_SETATTR],
		.rpc_argp	= &arg,
		.rpc_resp	= &res,
	};
	int status;

	nfs4_stateid_copy(&arg.stateid, &zero_stateid);

	status = nfs4_call_sync(server->client, server, &msg, &arg.seq_args, &res.seq_res, 1);
	if (status)
		dprintk("%s failed: %d\n", __func__, status);

	return status;
}