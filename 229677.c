static int _nfs4_proc_exchange_id(struct nfs_client *clp, const struct cred *cred,
			u32 sp4_how)
{
	struct rpc_task *task;
	struct nfs41_exchange_id_args *argp;
	struct nfs41_exchange_id_res *resp;
	unsigned long now = jiffies;
	int status;

	task = nfs4_run_exchange_id(clp, cred, sp4_how, NULL);
	if (IS_ERR(task))
		return PTR_ERR(task);

	argp = task->tk_msg.rpc_argp;
	resp = task->tk_msg.rpc_resp;
	status = task->tk_status;
	if (status  != 0)
		goto out;

	status = nfs4_check_cl_exchange_flags(resp->flags);
	if (status  != 0)
		goto out;

	status = nfs4_sp4_select_mode(clp, &resp->state_protect);
	if (status != 0)
		goto out;

	do_renew_lease(clp, now);

	clp->cl_clientid = resp->clientid;
	clp->cl_exchange_flags = resp->flags;
	clp->cl_seqid = resp->seqid;
	/* Client ID is not confirmed */
	if (!(resp->flags & EXCHGID4_FLAG_CONFIRMED_R))
		clear_bit(NFS4_SESSION_ESTABLISHED,
			  &clp->cl_session->session_state);

	if (clp->cl_serverscope != NULL &&
	    !nfs41_same_server_scope(clp->cl_serverscope,
				resp->server_scope)) {
		dprintk("%s: server_scope mismatch detected\n",
			__func__);
		set_bit(NFS4CLNT_SERVER_SCOPE_MISMATCH, &clp->cl_state);
	}

	swap(clp->cl_serverowner, resp->server_owner);
	swap(clp->cl_serverscope, resp->server_scope);
	swap(clp->cl_implid, resp->impl_id);

	/* Save the EXCHANGE_ID verifier session trunk tests */
	memcpy(clp->cl_confirm.data, argp->verifier.data,
	       sizeof(clp->cl_confirm.data));
out:
	trace_nfs4_exchange_id(clp, status);
	rpc_put_task(task);
	return status;
}