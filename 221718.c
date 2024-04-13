static void fuse_release_user_pages(struct fuse_args_pages *ap,
				    bool should_dirty)
{
	unsigned int i;

	for (i = 0; i < ap->num_pages; i++) {
		if (should_dirty)
			set_page_dirty_lock(ap->pages[i]);
		put_page(ap->pages[i]);
	}
}