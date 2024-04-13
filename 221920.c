static struct fuse_io_args *fuse_io_alloc(struct fuse_io_priv *io,
					  unsigned int npages)
{
	struct fuse_io_args *ia;

	ia = kzalloc(sizeof(*ia), GFP_KERNEL);
	if (ia) {
		ia->io = io;
		ia->ap.pages = fuse_pages_alloc(npages, GFP_KERNEL,
						&ia->ap.descs);
		if (!ia->ap.pages) {
			kfree(ia);
			ia = NULL;
		}
	}
	return ia;
}