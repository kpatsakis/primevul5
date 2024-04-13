static struct fuse_writepage_args *fuse_writepage_args_alloc(void)
{
	struct fuse_writepage_args *wpa;
	struct fuse_args_pages *ap;

	wpa = kzalloc(sizeof(*wpa), GFP_NOFS);
	if (wpa) {
		ap = &wpa->ia.ap;
		ap->num_pages = 0;
		ap->pages = fuse_pages_alloc(1, GFP_NOFS, &ap->descs);
		if (!ap->pages) {
			kfree(wpa);
			wpa = NULL;
		}
	}
	return wpa;

}