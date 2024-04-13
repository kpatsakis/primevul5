static void nfs4_sync_open_stateid(nfs4_stateid *dst,
		struct nfs4_state *state)
{
	__be32 seqid_open;
	u32 dst_seqid;
	int seq;

	for (;;) {
		if (!nfs4_valid_open_stateid(state))
			break;
		seq = read_seqbegin(&state->seqlock);
		if (!nfs4_state_match_open_stateid_other(state, dst)) {
			nfs4_stateid_copy(dst, &state->open_stateid);
			if (read_seqretry(&state->seqlock, seq))
				continue;
			break;
		}
		seqid_open = state->open_stateid.seqid;
		if (read_seqretry(&state->seqlock, seq))
			continue;

		dst_seqid = be32_to_cpu(dst->seqid);
		if ((s32)(dst_seqid - be32_to_cpu(seqid_open)) < 0)
			dst->seqid = seqid_open;
		break;
	}
}