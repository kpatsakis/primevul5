static int __mtab_find_umount_fs(struct libmnt_context *cxt,
			    const char *tgt,
			    struct libmnt_fs **pfs)
{
	int rc;
	struct libmnt_ns *ns_old;
	struct libmnt_table *mtab = NULL;
	struct libmnt_fs *fs;
	char *loopdev = NULL;

	assert(cxt);
	assert(tgt);
	assert(pfs);

	*pfs = NULL;
	DBG(CXT, ul_debugobj(cxt, " search %s in mountinfo", tgt));

	/*
	 * The mount table may be huge, and on systems with utab we have to
	 * merge userspace mount options into /proc/self/mountinfo. This all is
	 * expensive. The tab filter can be used to filter out entries, then a mount
	 * table and utab are very tiny files.
	 *
	 * The filter uses mnt_fs_streq_{target,srcpath} function where all
	 * paths should be absolute and canonicalized. This is done within
	 * mnt_context_get_mtab_for_target() where LABEL, UUID or symlinks are
	 * canonicalized. If --no-canonicalize is enabled than the target path
	 * is expected already canonical.
	 *
	 * Anyway it's better to read huge mount table than canonicalize target
	 * paths. It means we use the filter only if --no-canonicalize enabled.
	 *
	 * It also means that we have to read mount table from kernel
	 * (non-writable mtab).
	 */
	if (mnt_context_is_nocanonicalize(cxt) &&
	    !mnt_context_mtab_writable(cxt) && *tgt == '/')
		rc = mnt_context_get_mtab_for_target(cxt, &mtab, tgt);
	else
		rc = mnt_context_get_mtab(cxt, &mtab);

	if (rc) {
		DBG(CXT, ul_debugobj(cxt, "umount: failed to read mtab"));
		return rc;
	}

	if (mnt_table_get_nents(mtab) == 0) {
		DBG(CXT, ul_debugobj(cxt, "umount: mtab empty"));
		return 1;
	}

	ns_old = mnt_context_switch_target_ns(cxt);
	if (!ns_old)
		return -MNT_ERR_NAMESPACE;

try_loopdev:
	fs = mnt_table_find_target(mtab, tgt, MNT_ITER_BACKWARD);
	if (!fs && mnt_context_is_swapmatch(cxt)) {
		/*
		 * Maybe the option is source rather than target (sometimes
		 * people use e.g. "umount /dev/sda1")
		 */
		fs = mnt_table_find_source(mtab, tgt, MNT_ITER_BACKWARD);

		if (fs) {
			struct libmnt_fs *fs1 = mnt_table_find_target(mtab,
							mnt_fs_get_target(fs),
							MNT_ITER_BACKWARD);
			if (!fs1) {
				DBG(CXT, ul_debugobj(cxt, "mtab is broken?!?!"));
				rc = -EINVAL;
				goto err;
			}
			if (fs != fs1) {
				/* Something was stacked over `file' on the
				 * same mount point. */
				DBG(CXT, ul_debugobj(cxt,
						"umount: %s: %s is mounted "
						"over it on the same point",
						tgt, mnt_fs_get_source(fs1)));
				rc = -EINVAL;
				goto err;
			}
		}
	}

	if (!fs && !loopdev && mnt_context_is_swapmatch(cxt)) {
		/*
		 * Maybe the option is /path/file.img, try to convert to /dev/loopN
		 */
		struct stat st;

		if (mnt_stat_mountpoint(tgt, &st) == 0 && S_ISREG(st.st_mode)) {
			int count;
			struct libmnt_cache *cache = mnt_context_get_cache(cxt);
			const char *bf = cache ? mnt_resolve_path(tgt, cache) : tgt;

			count = loopdev_count_by_backing_file(bf, &loopdev);
			if (count == 1) {
				DBG(CXT, ul_debugobj(cxt,
					"umount: %s --> %s (retry)", tgt, loopdev));
				tgt = loopdev;
				goto try_loopdev;

			} else if (count > 1)
				DBG(CXT, ul_debugobj(cxt,
					"umount: warning: %s is associated "
					"with more than one loopdev", tgt));
		}
	}

	*pfs = fs;
	free(loopdev);
	if (!mnt_context_switch_ns(cxt, ns_old))
		return -MNT_ERR_NAMESPACE;

	DBG(CXT, ul_debugobj(cxt, "umount fs: %s", fs ? mnt_fs_get_target(fs) :
							"<not found>"));
	return fs ? 0 : 1;
err:
	free(loopdev);
	if (!mnt_context_switch_ns(cxt, ns_old))
		return -MNT_ERR_NAMESPACE;
	return rc;
}