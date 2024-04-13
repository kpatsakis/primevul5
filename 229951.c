static int nfs41_check_expired_locks(struct nfs4_state *state)
{
	int status, ret = NFS_OK;
	struct nfs4_lock_state *lsp, *prev = NULL;
	struct nfs_server *server = NFS_SERVER(state->inode);

	if (!test_bit(LK_STATE_IN_USE, &state->flags))
		goto out;

	spin_lock(&state->state_lock);
	list_for_each_entry(lsp, &state->lock_states, ls_locks) {
		if (test_bit(NFS_LOCK_INITIALIZED, &lsp->ls_flags)) {
			const struct cred *cred = lsp->ls_state->owner->so_cred;

			refcount_inc(&lsp->ls_count);
			spin_unlock(&state->state_lock);

			nfs4_put_lock_state(prev);
			prev = lsp;

			status = nfs41_test_and_free_expired_stateid(server,
					&lsp->ls_stateid,
					cred);
			trace_nfs4_test_lock_stateid(state, lsp, status);
			if (status == -NFS4ERR_EXPIRED ||
			    status == -NFS4ERR_BAD_STATEID) {
				clear_bit(NFS_LOCK_INITIALIZED, &lsp->ls_flags);
				lsp->ls_stateid.type = NFS4_INVALID_STATEID_TYPE;
				if (!recover_lost_locks)
					set_bit(NFS_LOCK_LOST, &lsp->ls_flags);
			} else if (status != NFS_OK) {
				ret = status;
				nfs4_put_lock_state(prev);
				goto out;
			}
			spin_lock(&state->state_lock);
		}
	}
	spin_unlock(&state->state_lock);
	nfs4_put_lock_state(prev);
out:
	return ret;
}