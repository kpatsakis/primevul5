static int do_umount(struct libmnt_context *cxt)
{
	int rc = 0, flags = 0;
	const char *src, *target;
	char *tgtbuf = NULL;

	assert(cxt);
	assert(cxt->fs);
	assert((cxt->flags & MNT_FL_MOUNTFLAGS_MERGED));
	assert(cxt->syscall_status == 1);

	if (cxt->helper)
		return exec_helper(cxt);

	src = mnt_fs_get_srcpath(cxt->fs);
	target = mnt_fs_get_target(cxt->fs);

	if (!target)
		return -EINVAL;

	DBG(CXT, ul_debugobj(cxt, "do umount"));

	if (mnt_context_is_restricted(cxt) && !mnt_context_is_fake(cxt)) {
		/*
		 * extra paranoia for non-root users
		 * -- chdir to the parent of the mountpoint and use NOFOLLOW
		 *    flag to avoid races and symlink attacks.
		 */
		if (umount_nofollow_support())
			flags |= UMOUNT_NOFOLLOW;

		rc = mnt_chdir_to_parent(target, &tgtbuf);
		if (rc)
			return rc;
		target = tgtbuf;
	}

	if (mnt_context_is_lazy(cxt))
		flags |= MNT_DETACH;

	if (mnt_context_is_force(cxt))
		flags |= MNT_FORCE;

	DBG(CXT, ul_debugobj(cxt, "umount(2) [target='%s', flags=0x%08x]%s",
				target, flags,
				mnt_context_is_fake(cxt) ? " (FAKE)" : ""));

	if (mnt_context_is_fake(cxt))
		rc = 0;
	else {
		rc = flags ? umount2(target, flags) : umount(target);
		if (rc < 0)
			cxt->syscall_status = -errno;
		free(tgtbuf);
	}

	/*
	 * try remount read-only
	 */
	if (rc < 0
	    && cxt->syscall_status == -EBUSY
	    && mnt_context_is_rdonly_umount(cxt)
	    && src) {

		mnt_context_set_mflags(cxt, (cxt->mountflags |
					     MS_REMOUNT | MS_RDONLY));
		mnt_context_enable_loopdel(cxt, FALSE);

		DBG(CXT, ul_debugobj(cxt,
			"umount(2) failed [errno=%d] -- trying to remount read-only",
			-cxt->syscall_status));

		rc = mount(src, mnt_fs_get_target(cxt->fs), NULL,
			    MS_REMOUNT | MS_RDONLY, NULL);
		if (rc < 0) {
			cxt->syscall_status = -errno;
			DBG(CXT, ul_debugobj(cxt,
				"read-only re-mount(2) failed [errno=%d]",
				-cxt->syscall_status));

			return -cxt->syscall_status;
		}
		cxt->syscall_status = 0;
		DBG(CXT, ul_debugobj(cxt, "read-only re-mount(2) success"));
		return 0;
	}

	if (rc < 0) {
		DBG(CXT, ul_debugobj(cxt, "umount(2) failed [errno=%d]",
			-cxt->syscall_status));
		return -cxt->syscall_status;
	}

	cxt->syscall_status = 0;
	DBG(CXT, ul_debugobj(cxt, "umount(2) success"));
	return 0;
}