static ssize_t fuse_async_req_send(struct fuse_mount *fm,
				   struct fuse_io_args *ia, size_t num_bytes)
{
	ssize_t err;
	struct fuse_io_priv *io = ia->io;

	spin_lock(&io->lock);
	kref_get(&io->refcnt);
	io->size += num_bytes;
	io->reqs++;
	spin_unlock(&io->lock);

	ia->ap.args.end = fuse_aio_complete_req;
	ia->ap.args.may_block = io->should_dirty;
	err = fuse_simple_background(fm, &ia->ap.args, GFP_KERNEL);
	if (err)
		fuse_aio_complete_req(fm, &ia->ap.args, err);

	return num_bytes;
}