int fuse_fsync_common(struct file *file, struct dentry *de, int datasync,
		      int isdir)
{
	struct inode *inode = de->d_inode;
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_file *ff = file->private_data;
	struct fuse_req *req;
	struct fuse_fsync_in inarg;
	int err;

	if (is_bad_inode(inode))
		return -EIO;

	if ((!isdir && fc->no_fsync) || (isdir && fc->no_fsyncdir))
		return 0;

	/*
	 * Start writeback against all dirty pages of the inode, then
	 * wait for all outstanding writes, before sending the FSYNC
	 * request.
	 */
	err = write_inode_now(inode, 0);
	if (err)
		return err;

	fuse_sync_writes(inode);

	req = fuse_get_req(fc);
	if (IS_ERR(req))
		return PTR_ERR(req);

	memset(&inarg, 0, sizeof(inarg));
	inarg.fh = ff->fh;
	inarg.fsync_flags = datasync ? 1 : 0;
	req->in.h.opcode = isdir ? FUSE_FSYNCDIR : FUSE_FSYNC;
	req->in.h.nodeid = get_node_id(inode);
	req->in.numargs = 1;
	req->in.args[0].size = sizeof(inarg);
	req->in.args[0].value = &inarg;
	fuse_request_send(fc, req);
	err = req->out.h.error;
	fuse_put_request(fc, req);
	if (err == -ENOSYS) {
		if (isdir)
			fc->no_fsyncdir = 1;
		else
			fc->no_fsync = 1;
		err = 0;
	}
	return err;
}