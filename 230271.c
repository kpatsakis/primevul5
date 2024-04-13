int nfs4_set_rw_stateid(nfs4_stateid *stateid,
		const struct nfs_open_context *ctx,
		const struct nfs_lock_context *l_ctx,
		fmode_t fmode)
{
	return nfs4_select_rw_stateid(ctx->state, fmode, l_ctx, stateid, NULL);
}