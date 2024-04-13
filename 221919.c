static void fuse_io_free(struct fuse_io_args *ia)
{
	kfree(ia->ap.pages);
	kfree(ia);
}