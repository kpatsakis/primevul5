nfs4_retry_setlk(struct nfs4_state *state, int cmd, struct file_lock *request)
{
	int status = -ERESTARTSYS;
	struct nfs4_lock_state *lsp = request->fl_u.nfs4_fl.owner;
	struct nfs_server *server = NFS_SERVER(state->inode);
	struct nfs_client *clp = server->nfs_client;
	wait_queue_head_t *q = &clp->cl_lock_waitq;
	struct nfs_lowner owner = { .clientid = clp->cl_clientid,
				    .id = lsp->ls_seqid.owner_id,
				    .s_dev = server->s_dev };
	struct nfs4_lock_waiter waiter = { .task  = current,
					   .inode = state->inode,
					   .owner = &owner};
	wait_queue_entry_t wait;

	/* Don't bother with waitqueue if we don't expect a callback */
	if (!test_bit(NFS_STATE_MAY_NOTIFY_LOCK, &state->flags))
		return nfs4_retry_setlk_simple(state, cmd, request);

	init_wait(&wait);
	wait.private = &waiter;
	wait.func = nfs4_wake_lock_waiter;

	while(!signalled()) {
		add_wait_queue(q, &wait);
		status = nfs4_proc_setlk(state, cmd, request);
		if ((status != -EAGAIN) || IS_SETLK(cmd)) {
			finish_wait(q, &wait);
			break;
		}

		status = -ERESTARTSYS;
		freezer_do_not_count();
		wait_woken(&wait, TASK_INTERRUPTIBLE, NFS4_LOCK_MAXTIMEOUT);
		freezer_count();
		finish_wait(q, &wait);
	}

	return status;
}