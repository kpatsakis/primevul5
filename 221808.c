ssize_t fuse_getxattr(struct inode *inode, const char *name, void *value,
		      size_t size)
{
	struct fuse_mount *fm = get_fuse_mount(inode);
	FUSE_ARGS(args);
	struct fuse_getxattr_in inarg;
	struct fuse_getxattr_out outarg;
	ssize_t ret;

	if (fm->fc->no_getxattr)
		return -EOPNOTSUPP;

	memset(&inarg, 0, sizeof(inarg));
	inarg.size = size;
	args.opcode = FUSE_GETXATTR;
	args.nodeid = get_node_id(inode);
	args.in_numargs = 2;
	args.in_args[0].size = sizeof(inarg);
	args.in_args[0].value = &inarg;
	args.in_args[1].size = strlen(name) + 1;
	args.in_args[1].value = name;
	/* This is really two different operations rolled into one */
	args.out_numargs = 1;
	if (size) {
		args.out_argvar = true;
		args.out_args[0].size = size;
		args.out_args[0].value = value;
	} else {
		args.out_args[0].size = sizeof(outarg);
		args.out_args[0].value = &outarg;
	}
	ret = fuse_simple_request(fm, &args);
	if (!ret && !size)
		ret = min_t(ssize_t, outarg.size, XATTR_SIZE_MAX);
	if (ret == -ENOSYS) {
		fm->fc->no_getxattr = 1;
		ret = -EOPNOTSUPP;
	}
	return ret;
}