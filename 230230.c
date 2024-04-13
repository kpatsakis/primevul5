static int nfs41_open_expired(struct nfs4_state_owner *sp, struct nfs4_state *state)
{
	int status;

	status = nfs41_check_delegation_stateid(state);
	if (status != NFS_OK)
		return status;
	nfs41_delegation_recover_stateid(state);

	status = nfs41_check_expired_locks(state);
	if (status != NFS_OK)
		return status;
	status = nfs41_check_open_stateid(state);
	if (status != NFS_OK)
		status = nfs4_open_expired(sp, state);
	return status;
}