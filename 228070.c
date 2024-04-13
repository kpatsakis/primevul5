int mnt_context_get_umount_excode(
			struct libmnt_context *cxt,
			int rc,
			char *buf,
			size_t bufsz)
{
	if (mnt_context_helper_executed(cxt))
		/*
		 * /sbin/umount.<type> called, return status
		 */
		return mnt_context_get_helper_status(cxt);

	if (rc == 0 && mnt_context_get_status(cxt) == 1)
		/*
		 * Libmount success && syscall success.
		 */
		return MNT_EX_SUCCESS;

	if (!mnt_context_syscall_called(cxt)) {
		/*
		 * libmount errors (extra library checks)
		 */
		if (rc == -EPERM && !mnt_context_tab_applied(cxt)) {
			/* failed to evaluate permissions because not found
			 * relevant entry in mtab */
			if (buf)
				snprintf(buf, bufsz, _("not mounted"));
			return MNT_EX_USAGE;
		}

		if (rc == -MNT_ERR_LOCK) {
			if (buf)
				snprintf(buf, bufsz, _("locking failed"));
			return MNT_EX_FILEIO;
		}

		if (rc == -MNT_ERR_NAMESPACE) {
			if (buf)
				snprintf(buf, bufsz, _("failed to switch namespace"));
			return MNT_EX_SYSERR;
		}
		return mnt_context_get_generic_excode(rc, buf, bufsz,
					_("umount failed: %m"));

	} if (mnt_context_get_syscall_errno(cxt) == 0) {
		/*
		 * umount(2) syscall success, but something else failed
		 * (probably error in mtab processing).
		 */
		if (rc == -MNT_ERR_LOCK) {
			if (buf)
				snprintf(buf, bufsz, _("filesystem was unmounted, but failed to update userspace mount table"));
			return MNT_EX_FILEIO;
		}

		if (rc == -MNT_ERR_NAMESPACE) {
			if (buf)
				snprintf(buf, bufsz, _("filesystem was unmounted, but failed to switch namespace back"));
			return MNT_EX_SYSERR;

		}

		if (rc < 0)
			return mnt_context_get_generic_excode(rc, buf, bufsz,
				_("filesystem was unmounted, but any subsequent operation failed: %m"));

		return MNT_EX_SOFTWARE;	/* internal error */
	}

	/*
	 * umount(2) errors
	 */
	if (buf) {
		int syserr = mnt_context_get_syscall_errno(cxt);

		switch (syserr) {
		case ENXIO:
			snprintf(buf, bufsz, _("invalid block device"));	/* ??? */
			break;
		case EINVAL:
			snprintf(buf, bufsz, _("not mounted"));
			break;
		case EIO:
			snprintf(buf, bufsz, _("can't write superblock"));
			break;
		case EBUSY:
			snprintf(buf, bufsz, _("target is busy"));
			break;
		case ENOENT:
			snprintf(buf, bufsz, _("no mount point specified"));
			break;
		case EPERM:
			snprintf(buf, bufsz, _("must be superuser to unmount"));
			break;
		case EACCES:
			snprintf(buf, bufsz, _("block devices are not permitted on filesystem"));
			break;
		default:
			return mnt_context_get_generic_excode(syserr, buf, bufsz,_("umount(2) system call failed: %m"));
		}
	}
	return MNT_EX_FAIL;
}