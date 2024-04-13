static void nfs_state_clear_delegation(struct nfs4_state *state)
{
	write_seqlock(&state->seqlock);
	nfs4_stateid_copy(&state->stateid, &state->open_stateid);
	clear_bit(NFS_DELEGATED_STATE, &state->flags);
	write_sequnlock(&state->seqlock);
}