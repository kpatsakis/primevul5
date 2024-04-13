static bool nfs4_refresh_open_old_stateid(nfs4_stateid *dst,
		struct nfs4_state *state)
{
	__be32 seqid_open;
	u32 dst_seqid;
	bool ret;
	int seq;

	for (;;) {
		ret = false;
		if (!nfs4_valid_open_stateid(state))
			break;
		seq = read_seqbegin(&state->seqlock);
		if (!nfs4_state_match_open_stateid_other(state, dst)) {
			if (read_seqretry(&state->seqlock, seq))
				continue;
			break;
		}
		seqid_open = state->open_stateid.seqid;
		if (read_seqretry(&state->seqlock, seq))
			continue;

		dst_seqid = be32_to_cpu(dst->seqid);
		if ((s32)(dst_seqid - be32_to_cpu(seqid_open)) >= 0)
			dst->seqid = cpu_to_be32(dst_seqid + 1);
		else
			dst->seqid = seqid_open;
		ret = true;
		break;
	}

	return ret;
}