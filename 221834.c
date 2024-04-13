static void fuse_short_read(struct inode *inode, u64 attr_ver, size_t num_read,
			    struct fuse_args_pages *ap)
{
	struct fuse_conn *fc = get_fuse_conn(inode);

	if (fc->writeback_cache) {
		/*
		 * A hole in a file. Some data after the hole are in page cache,
		 * but have not reached the client fs yet. So, the hole is not
		 * present there.
		 */
		int i;
		int start_idx = num_read >> PAGE_SHIFT;
		size_t off = num_read & (PAGE_SIZE - 1);

		for (i = start_idx; i < ap->num_pages; i++) {
			zero_user_segment(ap->pages[i], off, PAGE_SIZE);
			off = 0;
		}
	} else {
		loff_t pos = page_offset(ap->pages[0]) + num_read;
		fuse_read_update_size(inode, pos, attr_ver);
	}
}