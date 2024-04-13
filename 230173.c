static void nfs_state_log_out_of_order_open_stateid(struct nfs4_state *state,
		const nfs4_stateid *stateid)
{
	u32 state_seqid = be32_to_cpu(state->open_stateid.seqid);
	u32 stateid_seqid = be32_to_cpu(stateid->seqid);

	if (stateid_seqid == state_seqid + 1U ||
	    (stateid_seqid == 1U && state_seqid == 0xffffffffU))
		nfs_state_log_update_open_stateid(state);
	else
		set_bit(NFS_STATE_CHANGE_WAIT, &state->flags);
}