static void fuse_release_end(struct fuse_mount *fm, struct fuse_args *args,
			     int error)
{
	struct fuse_release_args *ra = container_of(args, typeof(*ra), args);

	iput(ra->inode);
	kfree(ra);
}