_nfs41_proc_secinfo_no_name(struct nfs_server *server, struct nfs_fh *fhandle,
		    struct nfs_fsinfo *info,
		    struct nfs4_secinfo_flavors *flavors, bool use_integrity)
{
	struct nfs41_secinfo_no_name_args args = {
		.style = SECINFO_STYLE_CURRENT_FH,
	};
	struct nfs4_secinfo_res res = {
		.flavors = flavors,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_SECINFO_NO_NAME],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};
	struct rpc_clnt *clnt = server->client;
	struct nfs4_call_sync_data data = {
		.seq_server = server,
		.seq_args = &args.seq_args,
		.seq_res = &res.seq_res,
	};
	struct rpc_task_setup task_setup = {
		.rpc_client = server->client,
		.rpc_message = &msg,
		.callback_ops = server->nfs_client->cl_mvops->call_sync_ops,
		.callback_data = &data,
		.flags = RPC_TASK_NO_ROUND_ROBIN,
	};
	const struct cred *cred = NULL;
	int status;

	if (use_integrity) {
		clnt = server->nfs_client->cl_rpcclient;
		task_setup.rpc_client = clnt;

		cred = nfs4_get_clid_cred(server->nfs_client);
		msg.rpc_cred = cred;
	}

	dprintk("--> %s\n", __func__);
	nfs4_init_sequence(&args.seq_args, &res.seq_res, 0, 0);
	status = nfs4_call_sync_custom(&task_setup);
	dprintk("<-- %s status=%d\n", __func__, status);

	put_cred(cred);

	return status;
}