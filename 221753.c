int fuse_setxattr(struct inode *inode, const char *name, const void *value,
		  size_t size, int flags)
{
	struct fuse_mount *fm = get_fuse_mount(inode);
	FUSE_ARGS(args);
	struct fuse_setxattr_in inarg;
	int err;

	if (fm->fc->no_setxattr)
		return -EOPNOTSUPP;

	memset(&inarg, 0, sizeof(inarg));
	inarg.size = size;
	inarg.flags = flags;
	args.opcode = FUSE_SETXATTR;
	args.nodeid = get_node_id(inode);
	args.in_numargs = 3;
	args.in_args[0].size = sizeof(inarg);
	args.in_args[0].value = &inarg;
	args.in_args[1].size = strlen(name) + 1;
	args.in_args[1].value = name;
	args.in_args[2].size = size;
	args.in_args[2].value = value;
	err = fuse_simple_request(fm, &args);
	if (err == -ENOSYS) {
		fm->fc->no_setxattr = 1;
		err = -EOPNOTSUPP;
	}
	if (!err) {
		fuse_invalidate_attr(inode);
		fuse_update_ctime(inode);
	}
	return err;
}