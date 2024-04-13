int fuse_open_common(struct inode *inode, struct file *file, bool isdir)
{
	struct fuse_conn *fc = get_fuse_conn(inode);
	int err;

	/* VFS checks this, but only _after_ ->open() */
	if (file->f_flags & O_DIRECT)
		return -EINVAL;

	err = generic_file_open(inode, file);
	if (err)
		return err;

	err = fuse_do_open(fc, get_node_id(inode), file, isdir);
	if (err)
		return err;

	fuse_finish_open(inode, file);

	return 0;
}