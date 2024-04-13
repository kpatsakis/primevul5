int fuse_removexattr(struct inode *inode, const char *name)
{
	struct fuse_mount *fm = get_fuse_mount(inode);
	FUSE_ARGS(args);
	int err;

	if (fm->fc->no_removexattr)
		return -EOPNOTSUPP;

	args.opcode = FUSE_REMOVEXATTR;
	args.nodeid = get_node_id(inode);
	args.in_numargs = 1;
	args.in_args[0].size = strlen(name) + 1;
	args.in_args[0].value = name;
	err = fuse_simple_request(fm, &args);
	if (err == -ENOSYS) {
		fm->fc->no_removexattr = 1;
		err = -EOPNOTSUPP;
	}
	if (!err) {
		fuse_invalidate_attr(inode);
		fuse_update_ctime(inode);
	}
	return err;
}