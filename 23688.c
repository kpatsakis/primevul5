static size_t fuse_send_write(struct fuse_req *req, struct file *file,
			      loff_t pos, size_t count, fl_owner_t owner)
{
	struct fuse_file *ff = file->private_data;
	struct fuse_conn *fc = ff->fc;
	struct fuse_write_in *inarg = &req->misc.write.in;

	fuse_write_fill(req, ff, pos, count);
	inarg->flags = file->f_flags;
	if (owner != NULL) {
		inarg->write_flags |= FUSE_WRITE_LOCKOWNER;
		inarg->lock_owner = fuse_lock_owner_id(fc, owner);
	}
	fuse_request_send(fc, req);
	return req->misc.write.out.size;
}