int fuse_fill_super_common(struct super_block *sb, struct fuse_fs_context *ctx)
{
	struct fuse_dev *fud = NULL;
	struct fuse_mount *fm = get_fuse_mount_super(sb);
	struct fuse_conn *fc = fm->fc;
	struct inode *root;
	struct dentry *root_dentry;
	int err;

	err = -EINVAL;
	if (sb->s_flags & SB_MANDLOCK)
		goto err;

	fuse_sb_defaults(sb);

	if (ctx->is_bdev) {
#ifdef CONFIG_BLOCK
		err = -EINVAL;
		if (!sb_set_blocksize(sb, ctx->blksize))
			goto err;
#endif
	} else {
		sb->s_blocksize = PAGE_SIZE;
		sb->s_blocksize_bits = PAGE_SHIFT;
	}

	sb->s_subtype = ctx->subtype;
	ctx->subtype = NULL;
	if (IS_ENABLED(CONFIG_FUSE_DAX)) {
		err = fuse_dax_conn_alloc(fc, ctx->dax_dev);
		if (err)
			goto err;
	}

	if (ctx->fudptr) {
		err = -ENOMEM;
		fud = fuse_dev_alloc_install(fc);
		if (!fud)
			goto err_free_dax;
	}

	fc->dev = sb->s_dev;
	fm->sb = sb;
	err = fuse_bdi_init(fc, sb);
	if (err)
		goto err_dev_free;

	/* Handle umasking inside the fuse code */
	if (sb->s_flags & SB_POSIXACL)
		fc->dont_mask = 1;
	sb->s_flags |= SB_POSIXACL;

	fc->default_permissions = ctx->default_permissions;
	fc->allow_other = ctx->allow_other;
	fc->user_id = ctx->user_id;
	fc->group_id = ctx->group_id;
	fc->legacy_opts_show = ctx->legacy_opts_show;
	fc->max_read = max_t(unsigned int, 4096, ctx->max_read);
	fc->destroy = ctx->destroy;
	fc->no_control = ctx->no_control;
	fc->no_force_umount = ctx->no_force_umount;

	err = -ENOMEM;
	root = fuse_get_root_inode(sb, ctx->rootmode);
	sb->s_d_op = &fuse_root_dentry_operations;
	root_dentry = d_make_root(root);
	if (!root_dentry)
		goto err_dev_free;
	/* Root dentry doesn't have .d_revalidate */
	sb->s_d_op = &fuse_dentry_operations;

	mutex_lock(&fuse_mutex);
	err = -EINVAL;
	if (ctx->fudptr && *ctx->fudptr)
		goto err_unlock;

	err = fuse_ctl_add_conn(fc);
	if (err)
		goto err_unlock;

	list_add_tail(&fc->entry, &fuse_conn_list);
	sb->s_root = root_dentry;
	if (ctx->fudptr)
		*ctx->fudptr = fud;
	mutex_unlock(&fuse_mutex);
	return 0;

 err_unlock:
	mutex_unlock(&fuse_mutex);
	dput(root_dentry);
 err_dev_free:
	if (fud)
		fuse_dev_free(fud);
 err_free_dax:
	if (IS_ENABLED(CONFIG_FUSE_DAX))
		fuse_dax_conn_free(fc);
 err:
	return err;
}