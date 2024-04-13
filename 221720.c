static int fuse_readdir_uncached(struct file *file, struct dir_context *ctx)
{
	int plus;
	ssize_t res;
	struct page *page;
	struct inode *inode = file_inode(file);
	struct fuse_mount *fm = get_fuse_mount(inode);
	struct fuse_io_args ia = {};
	struct fuse_args_pages *ap = &ia.ap;
	struct fuse_page_desc desc = { .length = PAGE_SIZE };
	u64 attr_version = 0;
	bool locked;

	page = alloc_page(GFP_KERNEL);
	if (!page)
		return -ENOMEM;

	plus = fuse_use_readdirplus(inode, ctx);
	ap->args.out_pages = true;
	ap->num_pages = 1;
	ap->pages = &page;
	ap->descs = &desc;
	if (plus) {
		attr_version = fuse_get_attr_version(fm->fc);
		fuse_read_args_fill(&ia, file, ctx->pos, PAGE_SIZE,
				    FUSE_READDIRPLUS);
	} else {
		fuse_read_args_fill(&ia, file, ctx->pos, PAGE_SIZE,
				    FUSE_READDIR);
	}
	locked = fuse_lock_inode(inode);
	res = fuse_simple_request(fm, &ap->args);
	fuse_unlock_inode(inode, locked);
	if (res >= 0) {
		if (!res) {
			struct fuse_file *ff = file->private_data;

			if (ff->open_flags & FOPEN_CACHE_DIR)
				fuse_readdir_cache_end(file, ctx->pos);
		} else if (plus) {
			res = parse_dirplusfile(page_address(page), res,
						file, ctx, attr_version);
		} else {
			res = parse_dirfile(page_address(page), res, file,
					    ctx);
		}
	}

	__free_page(page);
	fuse_invalidate_atime(inode);
	return res;
}