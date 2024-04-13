nfs4_atomic_open(struct inode *dir, struct nfs_open_context *ctx,
		int open_flags, struct iattr *attr, int *opened)
{
	struct nfs4_state *state;
	struct nfs4_label l = {0, 0, 0, NULL}, *label = NULL;

	label = nfs4_label_init_security(dir, ctx->dentry, attr, &l);

	/* Protect against concurrent sillydeletes */
	state = nfs4_do_open(dir, ctx, open_flags, attr, label, opened);

	nfs4_label_release_security(label);

	if (IS_ERR(state))
		return ERR_CAST(state);
	return state->inode;
}