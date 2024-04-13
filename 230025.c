nfs4_proc_create(struct inode *dir, struct dentry *dentry, struct iattr *sattr,
		 int flags)
{
	struct nfs_server *server = NFS_SERVER(dir);
	struct nfs4_label l, *ilabel = NULL;
	struct nfs_open_context *ctx;
	struct nfs4_state *state;
	int status = 0;

	ctx = alloc_nfs_open_context(dentry, FMODE_READ, NULL);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ilabel = nfs4_label_init_security(dir, dentry, sattr, &l);

	if (!(server->attr_bitmask[2] & FATTR4_WORD2_MODE_UMASK))
		sattr->ia_mode &= ~current_umask();
	state = nfs4_do_open(dir, ctx, flags, sattr, ilabel, NULL);
	if (IS_ERR(state)) {
		status = PTR_ERR(state);
		goto out;
	}
out:
	nfs4_label_release_security(ilabel);
	put_nfs_open_context(ctx);
	return status;
}