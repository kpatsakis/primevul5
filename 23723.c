static size_t fuse_send_read(struct fuse_req *req, struct file *file,
			     loff_t pos, size_t count, fl_owner_t owner)
{
	struct fuse_file *ff = file->private_data;
	struct fuse_conn *fc = ff->fc;

	fuse_read_fill(req, file, pos, count, FUSE_READ);
	if (owner != NULL) {
		struct fuse_read_in *inarg = &req->misc.read.in;

		inarg->read_flags |= FUSE_READ_LOCKOWNER;
		inarg->lock_owner = fuse_lock_owner_id(fc, owner);
	}
	fuse_request_send(fc, req);
	return req->out.args[0].size;
}