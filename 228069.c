int mnt_context_prepare_umount(struct libmnt_context *cxt)
{
	int rc;
	struct libmnt_ns *ns_old;

	if (!cxt || !cxt->fs || mnt_fs_is_swaparea(cxt->fs))
		return -EINVAL;
	if (!mnt_context_get_source(cxt) && !mnt_context_get_target(cxt))
		return -EINVAL;
	if (cxt->flags & MNT_FL_PREPARED)
		return 0;

	assert(cxt->helper_exec_status == 1);
	assert(cxt->syscall_status == 1);

	free(cxt->helper);	/* be paranoid */
	cxt->helper = NULL;
	cxt->action = MNT_ACT_UMOUNT;

	ns_old = mnt_context_switch_target_ns(cxt);
	if (!ns_old)
		return -MNT_ERR_NAMESPACE;

	rc = lookup_umount_fs(cxt);
	if (!rc)
		rc = mnt_context_merge_mflags(cxt);
	if (!rc)
		rc = evaluate_permissions(cxt);

	if (!rc && !cxt->helper) {

		if (cxt->user_mountflags & MNT_MS_HELPER)
			/* on helper= mount option based helper */
			rc = prepare_helper_from_options(cxt, "helper");

		if (!rc && !cxt->helper)
			/* on fstype based helper */
			rc = mnt_context_prepare_helper(cxt, "umount", NULL);
	}

	if (!rc && (cxt->user_mountflags & MNT_MS_LOOP))
		/* loop option explicitly specified in mtab, detach this loop */
		mnt_context_enable_loopdel(cxt, TRUE);

	if (!rc && mnt_context_is_loopdel(cxt) && cxt->fs) {
		const char *src = mnt_fs_get_srcpath(cxt->fs);

		if (src && (!is_loopdev(src) || loopdev_is_autoclear(src)))
			mnt_context_enable_loopdel(cxt, FALSE);
	}

	if (rc) {
		DBG(CXT, ul_debugobj(cxt, "umount: preparing failed"));
		return rc;
	}
	cxt->flags |= MNT_FL_PREPARED;

	if (!mnt_context_switch_ns(cxt, ns_old))
		return -MNT_ERR_NAMESPACE;

	return rc;
}