static bool nfs4_refresh_lock_old_stateid(nfs4_stateid *dst,
		struct nfs4_lock_state *lsp)
{
	struct nfs4_state *state = lsp->ls_state;
	bool ret = false;

	spin_lock(&state->state_lock);
	if (!nfs4_stateid_match_other(dst, &lsp->ls_stateid))
		goto out;
	if (!nfs4_stateid_is_newer(&lsp->ls_stateid, dst))
		nfs4_stateid_seqid_inc(dst);
	else
		dst->seqid = lsp->ls_stateid.seqid;
	ret = true;
out:
	spin_unlock(&state->state_lock);
	return ret;
}