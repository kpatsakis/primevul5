int update_open_stateid(struct nfs4_state *state,
		const nfs4_stateid *open_stateid,
		const nfs4_stateid *delegation,
		fmode_t fmode)
{
	struct nfs_server *server = NFS_SERVER(state->inode);
	struct nfs_client *clp = server->nfs_client;
	struct nfs_inode *nfsi = NFS_I(state->inode);
	struct nfs_delegation *deleg_cur;
	nfs4_stateid freeme = { };
	int ret = 0;

	fmode &= (FMODE_READ|FMODE_WRITE);

	rcu_read_lock();
	spin_lock(&state->owner->so_lock);
	if (open_stateid != NULL) {
		nfs_state_set_open_stateid(state, open_stateid, fmode, &freeme);
		ret = 1;
	}

	deleg_cur = nfs4_get_valid_delegation(state->inode);
	if (deleg_cur == NULL)
		goto no_delegation;

	spin_lock(&deleg_cur->lock);
	if (rcu_dereference(nfsi->delegation) != deleg_cur ||
	   test_bit(NFS_DELEGATION_RETURNING, &deleg_cur->flags) ||
	    (deleg_cur->type & fmode) != fmode)
		goto no_delegation_unlock;

	if (delegation == NULL)
		delegation = &deleg_cur->stateid;
	else if (!nfs4_stateid_match_other(&deleg_cur->stateid, delegation))
		goto no_delegation_unlock;

	nfs_mark_delegation_referenced(deleg_cur);
	nfs_state_set_delegation(state, &deleg_cur->stateid, fmode);
	ret = 1;
no_delegation_unlock:
	spin_unlock(&deleg_cur->lock);
no_delegation:
	if (ret)
		update_open_stateflags(state, fmode);
	spin_unlock(&state->owner->so_lock);
	rcu_read_unlock();

	if (test_bit(NFS_STATE_RECLAIM_NOGRACE, &state->flags))
		nfs4_schedule_state_manager(clp);
	if (freeme.type != 0)
		nfs4_test_and_free_stateid(server, &freeme,
				state->owner->so_cred);

	return ret;
}