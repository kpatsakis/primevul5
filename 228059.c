static int lookup_umount_fs_by_mountinfo(struct libmnt_context *cxt, const char *tgt)
{
	struct libmnt_fs *fs = NULL;
	int rc;

	assert(cxt);
	assert(cxt->fs);

	DBG(CXT, ul_debugobj(cxt, " lookup by mountinfo"));

	/* search */
	rc = __mtab_find_umount_fs(cxt, tgt, &fs);
	if (rc != 0)
		return rc;

	/* apply result */
	if (fs != cxt->fs) {
		mnt_fs_set_source(cxt->fs, NULL);
		mnt_fs_set_target(cxt->fs, NULL);

		if (!mnt_copy_fs(cxt->fs, fs)) {
			DBG(CXT, ul_debugobj(cxt, "  failed to copy FS"));
			return -errno;
		}
		DBG(CXT, ul_debugobj(cxt, "  mtab applied"));
	}

	cxt->flags |= MNT_FL_TAB_APPLIED;
	return 0;
}