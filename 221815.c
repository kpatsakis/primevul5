static int fuse_fill_super(struct super_block *sb, struct fs_context *fsc)
{
	struct fuse_fs_context *ctx = fsc->fs_private;
	struct file *file;
	int err;
	struct fuse_conn *fc;
	struct fuse_mount *fm;

	err = -EINVAL;
	file = fget(ctx->fd);
	if (!file)
		goto err;

	/*
	 * Require mount to happen from the same user namespace which
	 * opened /dev/fuse to prevent potential attacks.
	 */
	if ((file->f_op != &fuse_dev_operations) ||
	    (file->f_cred->user_ns != sb->s_user_ns))
		goto err_fput;
	ctx->fudptr = &file->private_data;

	fc = kmalloc(sizeof(*fc), GFP_KERNEL);
	err = -ENOMEM;
	if (!fc)
		goto err_fput;

	fm = kzalloc(sizeof(*fm), GFP_KERNEL);
	if (!fm) {
		kfree(fc);
		goto err_fput;
	}

	fuse_conn_init(fc, fm, sb->s_user_ns, &fuse_dev_fiq_ops, NULL);
	fc->release = fuse_free_conn;

	sb->s_fs_info = fm;

	err = fuse_fill_super_common(sb, ctx);
	if (err)
		goto err_put_conn;
	/*
	 * atomic_dec_and_test() in fput() provides the necessary
	 * memory barrier for file->private_data to be visible on all
	 * CPUs after this
	 */
	fput(file);
	fuse_send_init(get_fuse_mount_super(sb));
	return 0;

 err_put_conn:
	fuse_conn_put(fc);
	kfree(fm);
	sb->s_fs_info = NULL;
 err_fput:
	fput(file);
 err:
	return err;
}