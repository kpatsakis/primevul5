int fuse_do_open(struct fuse_mount *fm, u64 nodeid, struct file *file,
		 bool isdir)
{
	struct fuse_conn *fc = fm->fc;
	struct fuse_file *ff;
	int opcode = isdir ? FUSE_OPENDIR : FUSE_OPEN;

	ff = fuse_file_alloc(fm);
	if (!ff)
		return -ENOMEM;

	ff->fh = 0;
	/* Default for no-open */
	ff->open_flags = FOPEN_KEEP_CACHE | (isdir ? FOPEN_CACHE_DIR : 0);
	if (isdir ? !fc->no_opendir : !fc->no_open) {
		struct fuse_open_out outarg;
		int err;

		err = fuse_send_open(fm, nodeid, file, opcode, &outarg);
		if (!err) {
			ff->fh = outarg.fh;
			ff->open_flags = outarg.open_flags;

		} else if (err != -ENOSYS) {
			fuse_file_free(ff);
			return err;
		} else {
			if (isdir)
				fc->no_opendir = 1;
			else
				fc->no_open = 1;
		}
	}

	if (isdir)
		ff->open_flags &= ~FOPEN_DIRECT_IO;

	ff->nodeid = nodeid;
	file->private_data = ff;

	return 0;
}