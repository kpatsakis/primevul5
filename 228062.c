static int evaluate_permissions(struct libmnt_context *cxt)
{
	struct libmnt_table *fstab;
	unsigned long u_flags = 0;
	const char *tgt, *src, *optstr;
	int rc = 0, ok = 0;
	struct libmnt_fs *fs;

	assert(cxt);
	assert(cxt->fs);
	assert((cxt->flags & MNT_FL_MOUNTFLAGS_MERGED));

	if (!mnt_context_is_restricted(cxt))
		 return 0;		/* superuser mount */

	DBG(CXT, ul_debugobj(cxt, "umount: evaluating permissions"));

	if (!mnt_context_tab_applied(cxt)) {
		DBG(CXT, ul_debugobj(cxt,
				"cannot find %s in mtab and you are not root",
				mnt_fs_get_target(cxt->fs)));
		goto eperm;
	}

	if (cxt->user_mountflags & MNT_MS_UHELPER) {
		/* on uhelper= mount option based helper */
		rc = prepare_helper_from_options(cxt, "uhelper");
		if (rc)
			return rc;
		if (cxt->helper)
			return 0;	/* we'll call /sbin/umount.<uhelper> */
	}

	/*
	 * Check if this is a fuse mount for the current user,
	 * if so then unmounting is allowed
	 */
	if (is_fuse_usermount(cxt, &rc)) {
		DBG(CXT, ul_debugobj(cxt, "fuse user mount, umount is allowed"));
		return 0;
	}
	if (rc)
		return rc;

	/*
	 * User mounts have to be in /etc/fstab
	 */
	rc = mnt_context_get_fstab(cxt, &fstab);
	if (rc)
		return rc;

	tgt = mnt_fs_get_target(cxt->fs);
	src = mnt_fs_get_source(cxt->fs);

	if (mnt_fs_get_bindsrc(cxt->fs)) {
		src = mnt_fs_get_bindsrc(cxt->fs);
		DBG(CXT, ul_debugobj(cxt,
				"umount: using bind source: %s", src));
	}

	/* If fstab contains the two lines
	 *	/dev/sda1 /mnt/zip auto user,noauto  0 0
	 *	/dev/sda4 /mnt/zip auto user,noauto  0 0
	 * then "mount /dev/sda4" followed by "umount /mnt/zip" used to fail.
	 * So, we must not look for the file, but for the pair (dev,file) in fstab.
	  */
	fs = mnt_table_find_pair(fstab, src, tgt, MNT_ITER_FORWARD);
	if (!fs) {
		/*
		 * It's possible that there is /path/file.img in fstab and
		 * /dev/loop0 in mtab -- then we have to check the relation
		 * between loopdev and the file.
		 */
		fs = mnt_table_find_target(fstab, tgt, MNT_ITER_FORWARD);
		if (fs) {
			struct libmnt_cache *cache = mnt_context_get_cache(cxt);
			const char *sp = mnt_fs_get_srcpath(cxt->fs);		/* devname from mtab */
			const char *dev = sp && cache ? mnt_resolve_path(sp, cache) : sp;

			if (!dev || !is_associated_fs(dev, fs))
				fs = NULL;
		}
		if (!fs) {
			DBG(CXT, ul_debugobj(cxt,
					"umount %s: mtab disagrees with fstab",
					tgt));
			goto eperm;
		}
	}

	/*
	 * User mounting and unmounting is allowed only if fstab contains one
	 * of the options `user', `users' or `owner' or `group'.
	 *
	 * The option `users' allows arbitrary users to mount and unmount -
	 * this may be a security risk.
	 *
	 * The options `user', `owner' and `group' only allow unmounting by the
	 * user that mounted (visible in mtab).
	 */
	optstr = mnt_fs_get_user_options(fs);	/* FSTAB mount options! */
	if (!optstr)
		goto eperm;

	if (mnt_optstr_get_flags(optstr, &u_flags,
				mnt_get_builtin_optmap(MNT_USERSPACE_MAP)))
		goto eperm;

	if (u_flags & MNT_MS_USERS) {
		DBG(CXT, ul_debugobj(cxt,
			"umount: promiscuous setting ('users') in fstab"));
		return 0;
	}
	/*
	 * Check user=<username> setting from mtab if there is a user, owner or
	 * group option in /etc/fstab
	 */
	if (u_flags & (MNT_MS_USER | MNT_MS_OWNER | MNT_MS_GROUP)) {

		char *curr_user;
		char *mtab_user = NULL;
		size_t sz;
		struct libmnt_ns *ns_old;

		DBG(CXT, ul_debugobj(cxt,
				"umount: checking user=<username> from mtab"));

		ns_old = mnt_context_switch_origin_ns(cxt);
		if (!ns_old)
			return -MNT_ERR_NAMESPACE;

		curr_user = mnt_get_username(getuid());

		if (!mnt_context_switch_ns(cxt, ns_old)) {
			free(curr_user);
			return -MNT_ERR_NAMESPACE;
		}
		if (!curr_user) {
			DBG(CXT, ul_debugobj(cxt, "umount %s: cannot "
				"convert %d to username", tgt, getuid()));
			goto eperm;
		}

		/* get options from mtab */
		optstr = mnt_fs_get_user_options(cxt->fs);
		if (optstr && !mnt_optstr_get_option(optstr,
					"user", &mtab_user, &sz) && sz)
			ok = !strncmp(curr_user, mtab_user, sz);

		free(curr_user);
	}

	if (ok) {
		DBG(CXT, ul_debugobj(cxt, "umount %s is allowed", tgt));
		return 0;
	}
eperm:
	DBG(CXT, ul_debugobj(cxt, "umount is not allowed for you"));
	return -EPERM;
}