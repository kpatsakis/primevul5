static void fuse_writepage_free(struct fuse_writepage_args *wpa)
{
	struct fuse_args_pages *ap = &wpa->ia.ap;
	int i;

	for (i = 0; i < ap->num_pages; i++)
		__free_page(ap->pages[i]);

	if (wpa->ia.ff)
		fuse_file_put(wpa->ia.ff, false, false);

	kfree(ap->pages);
	kfree(wpa);
}