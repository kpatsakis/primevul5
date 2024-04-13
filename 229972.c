static int nfs41_check_delegation_stateid(struct nfs4_state *state)
{
	struct nfs_server *server = NFS_SERVER(state->inode);
	nfs4_stateid stateid;
	struct nfs_delegation *delegation;
	const struct cred *cred = NULL;
	int status, ret = NFS_OK;

	/* Get the delegation credential for use by test/free_stateid */
	rcu_read_lock();
	delegation = rcu_dereference(NFS_I(state->inode)->delegation);
	if (delegation == NULL) {
		rcu_read_unlock();
		nfs_state_clear_delegation(state);
		return NFS_OK;
	}

	spin_lock(&delegation->lock);
	nfs4_stateid_copy(&stateid, &delegation->stateid);

	if (!test_and_clear_bit(NFS_DELEGATION_TEST_EXPIRED,
				&delegation->flags)) {
		spin_unlock(&delegation->lock);
		rcu_read_unlock();
		return NFS_OK;
	}

	if (delegation->cred)
		cred = get_cred(delegation->cred);
	spin_unlock(&delegation->lock);
	rcu_read_unlock();
	status = nfs41_test_and_free_expired_stateid(server, &stateid, cred);
	trace_nfs4_test_delegation_stateid(state, NULL, status);
	if (status == -NFS4ERR_EXPIRED || status == -NFS4ERR_BAD_STATEID)
		nfs_finish_clear_delegation_stateid(state, &stateid);
	else
		ret = status;

	put_cred(cred);
	return ret;
}