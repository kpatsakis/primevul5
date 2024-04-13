static int _nfs41_test_stateid(struct nfs_server *server,
		nfs4_stateid *stateid,
		const struct cred *cred)
{
	int status;
	struct nfs41_test_stateid_args args = {
		.stateid = stateid,
	};
	struct nfs41_test_stateid_res res;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_TEST_STATEID],
		.rpc_argp = &args,
		.rpc_resp = &res,
		.rpc_cred = cred,
	};
	struct rpc_clnt *rpc_client = server->client;

	nfs4_state_protect(server->nfs_client, NFS_SP4_MACH_CRED_STATEID,
		&rpc_client, &msg);

	dprintk("NFS call  test_stateid %p\n", stateid);
	nfs4_init_sequence(&args.seq_args, &res.seq_res, 0, 1);
	status = nfs4_call_sync_sequence(rpc_client, server, &msg,
			&args.seq_args, &res.seq_res);
	if (status != NFS_OK) {
		dprintk("NFS reply test_stateid: failed, %d\n", status);
		return status;
	}
	dprintk("NFS reply test_stateid: succeeded, %d\n", -res.status);
	return -res.status;
}