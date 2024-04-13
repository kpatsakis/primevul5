int umd_load_blob(struct umd_info *info, const void *data, size_t len)
{
	struct vfsmount *mnt;

	if (WARN_ON_ONCE(info->wd.dentry || info->wd.mnt))
		return -EBUSY;

	mnt = blob_to_mnt(data, len, info->driver_name);
	if (IS_ERR(mnt))
		return PTR_ERR(mnt);

	info->wd.mnt = mnt;
	info->wd.dentry = mnt->mnt_root;
	return 0;
}