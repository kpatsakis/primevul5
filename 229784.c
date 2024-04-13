static bool nfs_need_update_open_stateid(struct nfs4_state *state,
		const nfs4_stateid *stateid)
{
	if (test_bit(NFS_OPEN_STATE, &state->flags) == 0 ||
	    !nfs4_stateid_match_other(stateid, &state->open_stateid)) {
		if (stateid->seqid == cpu_to_be32(1))
			nfs_state_log_update_open_stateid(state);
		else
			set_bit(NFS_STATE_CHANGE_WAIT, &state->flags);
		return true;
	}

	if (nfs4_stateid_is_newer(stateid, &state->open_stateid)) {
		nfs_state_log_out_of_order_open_stateid(state, stateid);
		return true;
	}
	return false;
}