void fuse_conn_destroy(struct fuse_mount *fm)
{
	struct fuse_conn *fc = fm->fc;

	if (fc->destroy)
		fuse_send_destroy(fm);

	fuse_abort_conn(fc);
	fuse_wait_aborted(fc);

	if (!list_empty(&fc->entry)) {
		mutex_lock(&fuse_mutex);
		list_del(&fc->entry);
		fuse_ctl_remove_conn(fc);
		mutex_unlock(&fuse_mutex);
	}
}