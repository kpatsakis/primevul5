static void fuse_readpages_end(struct fuse_mount *fm, struct fuse_args *args,
			       int err)
{
	int i;
	struct fuse_io_args *ia = container_of(args, typeof(*ia), ap.args);
	struct fuse_args_pages *ap = &ia->ap;
	size_t count = ia->read.in.size;
	size_t num_read = args->out_args[0].size;
	struct address_space *mapping = NULL;

	for (i = 0; mapping == NULL && i < ap->num_pages; i++)
		mapping = ap->pages[i]->mapping;

	if (mapping) {
		struct inode *inode = mapping->host;

		/*
		 * Short read means EOF. If file size is larger, truncate it
		 */
		if (!err && num_read < count)
			fuse_short_read(inode, ia->read.attr_ver, num_read, ap);

		fuse_invalidate_atime(inode);
	}

	for (i = 0; i < ap->num_pages; i++) {
		struct page *page = ap->pages[i];

		if (!err)
			SetPageUptodate(page);
		else
			SetPageError(page);
		unlock_page(page);
		put_page(page);
	}
	if (ia->ff)
		fuse_file_put(ia->ff, false, false);

	fuse_io_free(ia);
}