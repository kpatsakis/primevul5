static bool nfs4_sync_lock_stateid(nfs4_stateid *dst,
		struct nfs4_lock_state *lsp)
{
	struct nfs4_state *state = lsp->ls_state;
	bool ret;

	spin_lock(&state->state_lock);
	ret = !nfs4_stateid_match_other(dst, &lsp->ls_stateid);
	nfs4_stateid_copy(dst, &lsp->ls_stateid);
	spin_unlock(&state->state_lock);
	return ret;
}