static void nfs_state_set_delegation(struct nfs4_state *state,
		const nfs4_stateid *deleg_stateid,
		fmode_t fmode)
{
	/*
	 * Protect the call to nfs4_state_set_mode_locked and
	 * serialise the stateid update
	 */
	write_seqlock(&state->seqlock);
	nfs4_stateid_copy(&state->stateid, deleg_stateid);
	set_bit(NFS_DELEGATED_STATE, &state->flags);
	write_sequnlock(&state->seqlock);
}