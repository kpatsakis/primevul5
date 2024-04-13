static void nfs4_locku_prepare(struct rpc_task *task, void *data)
{
	struct nfs4_unlockdata *calldata = data;

	if (test_bit(NFS_CONTEXT_UNLOCK, &calldata->l_ctx->open_context->flags) &&
		nfs_async_iocounter_wait(task, calldata->l_ctx))
		return;

	if (nfs_wait_on_sequence(calldata->arg.seqid, task) != 0)
		goto out_wait;
	if (test_bit(NFS_LOCK_INITIALIZED, &calldata->lsp->ls_flags) == 0) {
		/* Note: exit _without_ running nfs4_locku_done */
		goto out_no_action;
	}
	calldata->timestamp = jiffies;
	if (nfs4_setup_sequence(calldata->server->nfs_client,
				&calldata->arg.seq_args,
				&calldata->res.seq_res,
				task) != 0)
		nfs_release_seqid(calldata->arg.seqid);
	return;
out_no_action:
	task->tk_action = NULL;
out_wait:
	nfs4_sequence_done(task, &calldata->res.seq_res);
}