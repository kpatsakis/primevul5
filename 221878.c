static int fuse_symlink(struct inode *dir, struct dentry *entry,
			const char *link)
{
	struct fuse_mount *fm = get_fuse_mount(dir);
	unsigned len = strlen(link) + 1;
	FUSE_ARGS(args);

	args.opcode = FUSE_SYMLINK;
	args.in_numargs = 2;
	args.in_args[0].size = entry->d_name.len + 1;
	args.in_args[0].value = entry->d_name.name;
	args.in_args[1].size = len;
	args.in_args[1].value = link;
	return create_new_entry(fm, &args, dir, entry, S_IFLNK);
}