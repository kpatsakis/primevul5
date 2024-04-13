void fuse_conn_put(struct fuse_conn *fc)
{
	if (refcount_dec_and_test(&fc->count)) {
		struct fuse_iqueue *fiq = &fc->iq;

		if (IS_ENABLED(CONFIG_FUSE_DAX))
			fuse_dax_conn_free(fc);
		if (fiq->ops->release)
			fiq->ops->release(fiq);
		put_pid_ns(fc->pid_ns);
		put_user_ns(fc->user_ns);
		fc->release(fc);
	}
}