_nfs4_proc_getdeviceinfo(struct nfs_server *server,
		struct pnfs_device *pdev,
		const struct cred *cred)
{
	struct nfs4_getdeviceinfo_args args = {
		.pdev = pdev,
		.notify_types = NOTIFY_DEVICEID4_CHANGE |
			NOTIFY_DEVICEID4_DELETE,
	};
	struct nfs4_getdeviceinfo_res res = {
		.pdev = pdev,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_GETDEVICEINFO],
		.rpc_argp = &args,
		.rpc_resp = &res,
		.rpc_cred = cred,
	};
	int status;

	dprintk("--> %s\n", __func__);
	status = nfs4_call_sync(server->client, server, &msg, &args.seq_args, &res.seq_res, 0);
	if (res.notification & ~args.notify_types)
		dprintk("%s: unsupported notification\n", __func__);
	if (res.notification != args.notify_types)
		pdev->nocache = 1;

	dprintk("<-- %s status=%d\n", __func__, status);

	return status;
}