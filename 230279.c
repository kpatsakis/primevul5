int nfs4_proc_setclientid(struct nfs_client *clp, u32 program,
		unsigned short port, const struct cred *cred,
		struct nfs4_setclientid_res *res)
{
	nfs4_verifier sc_verifier;
	struct nfs4_setclientid setclientid = {
		.sc_verifier = &sc_verifier,
		.sc_prog = program,
		.sc_clnt = clp,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_SETCLIENTID],
		.rpc_argp = &setclientid,
		.rpc_resp = res,
		.rpc_cred = cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = clp->cl_rpcclient,
		.rpc_message = &msg,
		.callback_ops = &nfs4_setclientid_ops,
		.callback_data = &setclientid,
		.flags = RPC_TASK_TIMEOUT | RPC_TASK_NO_ROUND_ROBIN,
	};
	unsigned long now = jiffies;
	int status;

	/* nfs_client_id4 */
	nfs4_init_boot_verifier(clp, &sc_verifier);

	if (test_bit(NFS_CS_MIGRATION, &clp->cl_flags))
		status = nfs4_init_uniform_client_string(clp);
	else
		status = nfs4_init_nonuniform_client_string(clp);

	if (status)
		goto out;

	/* cb_client4 */
	setclientid.sc_netid_len =
				nfs4_init_callback_netid(clp,
						setclientid.sc_netid,
						sizeof(setclientid.sc_netid));
	setclientid.sc_uaddr_len = scnprintf(setclientid.sc_uaddr,
				sizeof(setclientid.sc_uaddr), "%s.%u.%u",
				clp->cl_ipaddr, port >> 8, port & 255);

	dprintk("NFS call  setclientid auth=%s, '%s'\n",
		clp->cl_rpcclient->cl_auth->au_ops->au_name,
		clp->cl_owner_id);

	status = nfs4_call_sync_custom(&task_setup_data);
	if (setclientid.sc_cred) {
		kfree(clp->cl_acceptor);
		clp->cl_acceptor = rpcauth_stringify_acceptor(setclientid.sc_cred);
		put_rpccred(setclientid.sc_cred);
	}

	if (status == 0)
		do_renew_lease(clp, now);
out:
	trace_nfs4_setclientid(clp, status);
	dprintk("NFS reply setclientid: %d\n", status);
	return status;
}