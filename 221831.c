static void fuse_lookup_init(struct fuse_conn *fc, struct fuse_args *args,
			     u64 nodeid, const struct qstr *name,
			     struct fuse_entry_out *outarg)
{
	memset(outarg, 0, sizeof(struct fuse_entry_out));
	args->opcode = FUSE_LOOKUP;
	args->nodeid = nodeid;
	args->in_numargs = 1;
	args->in_args[0].size = name->len + 1;
	args->in_args[0].value = name->name;
	args->out_numargs = 1;
	args->out_args[0].size = sizeof(struct fuse_entry_out);
	args->out_args[0].value = outarg;
}