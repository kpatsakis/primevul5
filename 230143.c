static int nfs4_open_recover(struct nfs4_opendata *opendata, struct nfs4_state *state)
{
	int ret;

	/* memory barrier prior to reading state->n_* */
	smp_rmb();
	ret = nfs4_open_recover_helper(opendata, FMODE_READ|FMODE_WRITE);
	if (ret != 0)
		return ret;
	ret = nfs4_open_recover_helper(opendata, FMODE_WRITE);
	if (ret != 0)
		return ret;
	ret = nfs4_open_recover_helper(opendata, FMODE_READ);
	if (ret != 0)
		return ret;
	/*
	 * We may have performed cached opens for all three recoveries.
	 * Check if we need to update the current stateid.
	 */
	if (test_bit(NFS_DELEGATED_STATE, &state->flags) == 0 &&
	    !nfs4_stateid_match(&state->stateid, &state->open_stateid)) {
		write_seqlock(&state->seqlock);
		if (test_bit(NFS_DELEGATED_STATE, &state->flags) == 0)
			nfs4_stateid_copy(&state->stateid, &state->open_stateid);
		write_sequnlock(&state->seqlock);
	}
	return 0;
}