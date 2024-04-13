nfs4_state_find_open_context(struct nfs4_state *state)
{
	struct nfs_open_context *ctx;

	ctx = nfs4_state_find_open_context_mode(state, FMODE_READ|FMODE_WRITE);
	if (!IS_ERR(ctx))
		return ctx;
	ctx = nfs4_state_find_open_context_mode(state, FMODE_WRITE);
	if (!IS_ERR(ctx))
		return ctx;
	return nfs4_state_find_open_context_mode(state, FMODE_READ);
}