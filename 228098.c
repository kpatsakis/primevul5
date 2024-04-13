int mnt_context_next_umount(struct libmnt_context *cxt,
			   struct libmnt_iter *itr,
			   struct libmnt_fs **fs,
			   int *mntrc,
			   int *ignored)
{
	struct libmnt_table *mtab;
	const char *tgt;
	int rc;

	if (ignored)
		*ignored = 0;
	if (mntrc)
		*mntrc = 0;

	if (!cxt || !fs || !itr)
		return -EINVAL;

	rc = mnt_context_get_mtab(cxt, &mtab);
	cxt->mtab = NULL;		/* do not reset mtab */
	mnt_reset_context(cxt);

	if (rc)
		return rc;

	cxt->mtab = mtab;

	do {
		rc = mnt_table_next_fs(mtab, itr, fs);
		if (rc != 0)
			return rc;	/* no more filesystems (or error) */

		tgt = mnt_fs_get_target(*fs);
	} while (!tgt);

	DBG(CXT, ul_debugobj(cxt, "next-umount: trying %s [fstype: %s, t-pattern: %s, options: %s, O-pattern: %s]", tgt,
				 mnt_fs_get_fstype(*fs), cxt->fstype_pattern, mnt_fs_get_options(*fs), cxt->optstr_pattern));

	/* ignore filesystems which don't match options patterns */
	if ((cxt->fstype_pattern && !mnt_fs_match_fstype(*fs,
					cxt->fstype_pattern)) ||

	/* ignore filesystems which don't match type patterns */
	   (cxt->optstr_pattern && !mnt_fs_match_options(*fs,
					cxt->optstr_pattern))) {
		if (ignored)
			*ignored = 1;

		DBG(CXT, ul_debugobj(cxt, "next-umount: not-match"));
		return 0;
	}

	rc = mnt_context_set_fs(cxt, *fs);
	if (rc)
		return rc;
	rc = mnt_context_umount(cxt);
	if (mntrc)
		*mntrc = rc;
	return 0;
}