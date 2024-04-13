bool fuse_mount_remove(struct fuse_mount *fm)
{
	struct fuse_conn *fc = fm->fc;
	bool last = false;

	down_write(&fc->killsb);
	list_del_init(&fm->fc_entry);
	if (list_empty(&fc->mounts))
		last = true;
	up_write(&fc->killsb);

	return last;
}