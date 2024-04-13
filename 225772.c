static struct vfsmount *blob_to_mnt(const void *data, size_t len, const char *name)
{
	struct file_system_type *type;
	struct vfsmount *mnt;
	struct file *file;
	ssize_t written;
	loff_t pos = 0;

	type = get_fs_type("tmpfs");
	if (!type)
		return ERR_PTR(-ENODEV);

	mnt = kern_mount(type);
	put_filesystem(type);
	if (IS_ERR(mnt))
		return mnt;

	file = file_open_root(mnt->mnt_root, mnt, name, O_CREAT | O_WRONLY, 0700);
	if (IS_ERR(file)) {
		mntput(mnt);
		return ERR_CAST(file);
	}

	written = kernel_write(file, data, len, &pos);
	if (written != len) {
		int err = written;
		if (err >= 0)
			err = -ENOMEM;
		filp_close(file, NULL);
		mntput(mnt);
		return ERR_PTR(err);
	}

	fput(file);

	/* Flush delayed fput so exec can open the file read-only */
	flush_delayed_fput();
	task_work_run();
	return mnt;
}