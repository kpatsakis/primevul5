static int nfs41_check_open_stateid(struct nfs4_state *state)
{
	struct nfs_server *server = NFS_SERVER(state->inode);
	nfs4_stateid *stateid = &state->open_stateid;
	const struct cred *cred = state->owner->so_cred;
	int status;

	if (test_bit(NFS_OPEN_STATE, &state->flags) == 0)
		return -NFS4ERR_BAD_STATEID;
	status = nfs41_test_and_free_expired_stateid(server, stateid, cred);
	trace_nfs4_test_open_stateid(state, NULL, status);
	if (status == -NFS4ERR_EXPIRED || status == -NFS4ERR_BAD_STATEID) {
		nfs_state_clear_open_state_flags(state);
		stateid->type = NFS4_INVALID_STATEID_TYPE;
		return status;
	}
	if (nfs_open_stateid_recover_openmode(state))
		return -NFS4ERR_OPENMODE;
	return NFS_OK;
}