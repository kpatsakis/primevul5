static void nfs_clear_open_stateid(struct nfs4_state *state,
	nfs4_stateid *arg_stateid,
	nfs4_stateid *stateid, fmode_t fmode)
{
	write_seqlock(&state->seqlock);
	/* Ignore, if the CLOSE argment doesn't match the current stateid */
	if (nfs4_state_match_open_stateid_other(state, arg_stateid))
		nfs_clear_open_stateid_locked(state, stateid, fmode);
	write_sequnlock(&state->seqlock);
	if (test_bit(NFS_STATE_RECLAIM_NOGRACE, &state->flags))
		nfs4_schedule_state_manager(state->owner->so_server->nfs_client);
}