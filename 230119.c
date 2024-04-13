nfs4_state_find_open_context_mode(struct nfs4_state *state, fmode_t mode)
{
	struct nfs_inode *nfsi = NFS_I(state->inode);
	struct nfs_open_context *ctx;

	rcu_read_lock();
	list_for_each_entry_rcu(ctx, &nfsi->open_files, list) {
		if (ctx->state != state)
			continue;
		if ((ctx->mode & mode) != mode)
			continue;
		if (!get_nfs_open_context(ctx))
			continue;
		rcu_read_unlock();
		return ctx;
	}
	rcu_read_unlock();
	return ERR_PTR(-ENOENT);
}