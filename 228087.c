int mnt_context_do_umount(struct libmnt_context *cxt)
{
	int rc;
	struct libmnt_ns *ns_old;

	assert(cxt);
	assert(cxt->fs);
	assert(cxt->helper_exec_status == 1);
	assert(cxt->syscall_status == 1);
	assert((cxt->flags & MNT_FL_PREPARED));
	assert((cxt->action == MNT_ACT_UMOUNT));
	assert((cxt->flags & MNT_FL_MOUNTFLAGS_MERGED));

	ns_old = mnt_context_switch_target_ns(cxt);
	if (!ns_old)
		return -MNT_ERR_NAMESPACE;

	rc = do_umount(cxt);
	if (rc)
		goto end;

	if (mnt_context_get_status(cxt) && !mnt_context_is_fake(cxt)) {
		/*
		 * Umounted, do some post-umount operations
		 *	- remove loopdev
		 *	- refresh in-memory mtab stuff if remount rather than
		 *	  umount has been performed
		 */
		if (mnt_context_is_loopdel(cxt)
		    && !(cxt->mountflags & MS_REMOUNT))
			rc = mnt_context_delete_loopdev(cxt);

		if (!mnt_context_is_nomtab(cxt)
		    && mnt_context_get_status(cxt)
		    && !cxt->helper
		    && mnt_context_is_rdonly_umount(cxt)
		    && (cxt->mountflags & MS_REMOUNT)) {

			/* use "remount" instead of "umount" in /etc/mtab */
			if (!rc && cxt->update && mnt_context_mtab_writable(cxt))
				rc = mnt_update_set_fs(cxt->update,
						       cxt->mountflags, NULL, cxt->fs);
		}
	}
end:
	if (!mnt_context_switch_ns(cxt, ns_old))
		return -MNT_ERR_NAMESPACE;

	return rc;
}