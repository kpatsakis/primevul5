static int lookup_umount_fs(struct libmnt_context *cxt)
{
	const char *tgt;
	int rc = 0;

	assert(cxt);
	assert(cxt->fs);

	DBG(CXT, ul_debugobj(cxt, "umount: lookup FS"));

	tgt = mnt_fs_get_target(cxt->fs);
	if (!tgt) {
		DBG(CXT, ul_debugobj(cxt, " undefined target"));
		return -EINVAL;
	}

	/* try get fs type by statfs() */
	rc = lookup_umount_fs_by_statfs(cxt, tgt);
	if (rc <= 0)
		return rc;

	/* get complete fs from fs entry from mountinfo */
	rc = lookup_umount_fs_by_mountinfo(cxt, tgt);
	if (rc <= 0)
		return rc;

	DBG(CXT, ul_debugobj(cxt, " cannot find '%s'", tgt));
	return 0;	/* this is correct! */
}