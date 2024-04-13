static int fuse_update_get_attr(struct inode *inode, struct file *file,
				struct kstat *stat, u32 request_mask,
				unsigned int flags)
{
	struct fuse_inode *fi = get_fuse_inode(inode);
	int err = 0;
	bool sync;

	if (flags & AT_STATX_FORCE_SYNC)
		sync = true;
	else if (flags & AT_STATX_DONT_SYNC)
		sync = false;
	else if (request_mask & READ_ONCE(fi->inval_mask))
		sync = true;
	else
		sync = time_before64(fi->i_time, get_jiffies_64());

	if (sync) {
		forget_all_cached_acls(inode);
		err = fuse_do_getattr(inode, stat, file);
	} else if (stat) {
		generic_fillattr(inode, stat);
		stat->mode = fi->orig_i_mode;
		stat->ino = fi->orig_ino;
	}

	return err;
}