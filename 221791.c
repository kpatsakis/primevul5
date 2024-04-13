static ssize_t __fuse_copy_file_range(struct file *file_in, loff_t pos_in,
				      struct file *file_out, loff_t pos_out,
				      size_t len, unsigned int flags)
{
	struct fuse_file *ff_in = file_in->private_data;
	struct fuse_file *ff_out = file_out->private_data;
	struct inode *inode_in = file_inode(file_in);
	struct inode *inode_out = file_inode(file_out);
	struct fuse_inode *fi_out = get_fuse_inode(inode_out);
	struct fuse_mount *fm = ff_in->fm;
	struct fuse_conn *fc = fm->fc;
	FUSE_ARGS(args);
	struct fuse_copy_file_range_in inarg = {
		.fh_in = ff_in->fh,
		.off_in = pos_in,
		.nodeid_out = ff_out->nodeid,
		.fh_out = ff_out->fh,
		.off_out = pos_out,
		.len = len,
		.flags = flags
	};
	struct fuse_write_out outarg;
	ssize_t err;
	/* mark unstable when write-back is not used, and file_out gets
	 * extended */
	bool is_unstable = (!fc->writeback_cache) &&
			   ((pos_out + len) > inode_out->i_size);

	if (fc->no_copy_file_range)
		return -EOPNOTSUPP;

	if (file_inode(file_in)->i_sb != file_inode(file_out)->i_sb)
		return -EXDEV;

	inode_lock(inode_in);
	err = fuse_writeback_range(inode_in, pos_in, pos_in + len - 1);
	inode_unlock(inode_in);
	if (err)
		return err;

	inode_lock(inode_out);

	err = file_modified(file_out);
	if (err)
		goto out;

	/*
	 * Write out dirty pages in the destination file before sending the COPY
	 * request to userspace.  After the request is completed, truncate off
	 * pages (including partial ones) from the cache that have been copied,
	 * since these contain stale data at that point.
	 *
	 * This should be mostly correct, but if the COPY writes to partial
	 * pages (at the start or end) and the parts not covered by the COPY are
	 * written through a memory map after calling fuse_writeback_range(),
	 * then these partial page modifications will be lost on truncation.
	 *
	 * It is unlikely that someone would rely on such mixed style
	 * modifications.  Yet this does give less guarantees than if the
	 * copying was performed with write(2).
	 *
	 * To fix this a i_mmap_sem style lock could be used to prevent new
	 * faults while the copy is ongoing.
	 */
	err = fuse_writeback_range(inode_out, pos_out, pos_out + len - 1);
	if (err)
		goto out;

	if (is_unstable)
		set_bit(FUSE_I_SIZE_UNSTABLE, &fi_out->state);

	args.opcode = FUSE_COPY_FILE_RANGE;
	args.nodeid = ff_in->nodeid;
	args.in_numargs = 1;
	args.in_args[0].size = sizeof(inarg);
	args.in_args[0].value = &inarg;
	args.out_numargs = 1;
	args.out_args[0].size = sizeof(outarg);
	args.out_args[0].value = &outarg;
	err = fuse_simple_request(fm, &args);
	if (err == -ENOSYS) {
		fc->no_copy_file_range = 1;
		err = -EOPNOTSUPP;
	}
	if (err)
		goto out;

	truncate_inode_pages_range(inode_out->i_mapping,
				   ALIGN_DOWN(pos_out, PAGE_SIZE),
				   ALIGN(pos_out + outarg.size, PAGE_SIZE) - 1);

	if (fc->writeback_cache) {
		fuse_write_update_size(inode_out, pos_out + outarg.size);
		file_update_time(file_out);
	}

	fuse_invalidate_attr(inode_out);

	err = outarg.size;
out:
	if (is_unstable)
		clear_bit(FUSE_I_SIZE_UNSTABLE, &fi_out->state);

	inode_unlock(inode_out);
	file_accessed(file_in);

	return err;
}