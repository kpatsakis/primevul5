int umd_unload_blob(struct umd_info *info)
{
	if (WARN_ON_ONCE(!info->wd.mnt ||
			 !info->wd.dentry ||
			 info->wd.mnt->mnt_root != info->wd.dentry))
		return -EINVAL;

	kern_unmount(info->wd.mnt);
	info->wd.mnt = NULL;
	info->wd.dentry = NULL;
	return 0;
}