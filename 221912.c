static int fuse_mknod(struct inode *dir, struct dentry *entry, umode_t mode,
		      dev_t rdev)
{
	struct fuse_mknod_in inarg;
	struct fuse_mount *fm = get_fuse_mount(dir);
	FUSE_ARGS(args);

	if (!fm->fc->dont_mask)
		mode &= ~current_umask();

	memset(&inarg, 0, sizeof(inarg));
	inarg.mode = mode;
	inarg.rdev = new_encode_dev(rdev);
	inarg.umask = current_umask();
	args.opcode = FUSE_MKNOD;
	args.in_numargs = 2;
	args.in_args[0].size = sizeof(inarg);
	args.in_args[0].value = &inarg;
	args.in_args[1].size = entry->d_name.len + 1;
	args.in_args[1].value = entry->d_name.name;
	return create_new_entry(fm, &args, dir, entry, mode);
}