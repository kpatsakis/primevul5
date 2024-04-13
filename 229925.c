static int _nfs4_proc_secinfo(struct inode *dir, const struct qstr *name, struct nfs4_secinfo_flavors *flavors, bool use_integrity)
{
	int status;
	struct rpc_clnt *clnt = NFS_SERVER(dir)->client;
	struct nfs_client *clp = NFS_SERVER(dir)->nfs_client;
	struct nfs4_secinfo_arg args = {
		.dir_fh = NFS_FH(dir),
		.name   = name,
	};
	struct nfs4_secinfo_res res = {
		.flavors     = flavors,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_SECINFO],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};
	struct nfs4_call_sync_data data = {
		.seq_server = NFS_SERVER(dir),
		.seq_args = &args.seq_args,
		.seq_res = &res.seq_res,
	};
	struct rpc_task_setup task_setup = {
		.rpc_client = clnt,
		.rpc_message = &msg,
		.callback_ops = clp->cl_mvops->call_sync_ops,
		.callback_data = &data,
		.flags = RPC_TASK_NO_ROUND_ROBIN,
	};
	const struct cred *cred = NULL;

	if (use_integrity) {
		clnt = clp->cl_rpcclient;
		task_setup.rpc_client = clnt;

		cred = nfs4_get_clid_cred(clp);
		msg.rpc_cred = cred;
	}

	dprintk("NFS call  secinfo %s\n", name->name);

	nfs4_state_protect(clp, NFS_SP4_MACH_CRED_SECINFO, &clnt, &msg);
	nfs4_init_sequence(&args.seq_args, &res.seq_res, 0, 0);
	status = nfs4_call_sync_custom(&task_setup);

	dprintk("NFS reply  secinfo: %d\n", status);

	put_cred(cred);
	return status;
}