static void nfs_clear_open_stateid_locked(struct nfs4_state *state,
		nfs4_stateid *stateid, fmode_t fmode)
{
	clear_bit(NFS_O_RDWR_STATE, &state->flags);
	switch (fmode & (FMODE_READ|FMODE_WRITE)) {
	case FMODE_WRITE:
		clear_bit(NFS_O_RDONLY_STATE, &state->flags);
		break;
	case FMODE_READ:
		clear_bit(NFS_O_WRONLY_STATE, &state->flags);
		break;
	case 0:
		clear_bit(NFS_O_RDONLY_STATE, &state->flags);
		clear_bit(NFS_O_WRONLY_STATE, &state->flags);
		clear_bit(NFS_OPEN_STATE, &state->flags);
	}
	if (stateid == NULL)
		return;
	/* Handle OPEN+OPEN_DOWNGRADE races */
	if (nfs4_stateid_match_other(stateid, &state->open_stateid) &&
	    !nfs4_stateid_is_newer(stateid, &state->open_stateid)) {
		nfs_resync_open_stateid_locked(state);
		goto out;
	}
	if (test_bit(NFS_DELEGATED_STATE, &state->flags) == 0)
		nfs4_stateid_copy(&state->stateid, stateid);
	nfs4_stateid_copy(&state->open_stateid, stateid);
	trace_nfs4_open_stateid_update(state->inode, stateid, 0);
out:
	nfs_state_log_update_open_stateid(state);
}