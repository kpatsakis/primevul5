static void fuse_write_args_fill(struct fuse_io_args *ia, struct fuse_file *ff,
				 loff_t pos, size_t count)
{
	struct fuse_args *args = &ia->ap.args;

	ia->write.in.fh = ff->fh;
	ia->write.in.offset = pos;
	ia->write.in.size = count;
	args->opcode = FUSE_WRITE;
	args->nodeid = ff->nodeid;
	args->in_numargs = 2;
	if (ff->fm->fc->minor < 9)
		args->in_args[0].size = FUSE_COMPAT_WRITE_IN_SIZE;
	else
		args->in_args[0].size = sizeof(ia->write.in);
	args->in_args[0].value = &ia->write.in;
	args->in_args[1].size = count;
	args->out_numargs = 1;
	args->out_args[0].size = sizeof(ia->write.out);
	args->out_args[0].value = &ia->write.out;
}