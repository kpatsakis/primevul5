static struct vfsmount *trace_automount(struct dentry *mntpt, void *ingore)
{
	struct vfsmount *mnt;
	struct file_system_type *type;

	/*
	 * To maintain backward compatibility for tools that mount
	 * debugfs to get to the tracing facility, tracefs is automatically
	 * mounted to the debugfs/tracing directory.
	 */
	type = get_fs_type("tracefs");
	if (!type)
		return NULL;
	mnt = vfs_submount(mntpt, type, "tracefs", NULL);
	put_filesystem(type);
	if (IS_ERR(mnt))
		return NULL;
	mntget(mnt);

	return mnt;
}