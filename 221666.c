static ssize_t fuse_send_read(struct fuse_io_args *ia, loff_t pos, size_t count,
			      fl_owner_t owner)
{
	struct file *file = ia->io->iocb->ki_filp;
	struct fuse_file *ff = file->private_data;
	struct fuse_mount *fm = ff->fm;

	fuse_read_args_fill(ia, file, pos, count, FUSE_READ);
	if (owner != NULL) {
		ia->read.in.read_flags |= FUSE_READ_LOCKOWNER;
		ia->read.in.lock_owner = fuse_lock_owner_id(fm->fc, owner);
	}

	if (ia->io->async)
		return fuse_async_req_send(fm, ia, count);

	return fuse_simple_request(fm, &ia->ap.args);
}