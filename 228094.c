static int prepare_helper_from_options(struct libmnt_context *cxt,
				       const char *name)
{
	char *suffix = NULL;
	const char *opts;
	size_t valsz;
	int rc;

	if (mnt_context_is_nohelpers(cxt))
		return 0;

	opts = mnt_fs_get_user_options(cxt->fs);
	if (!opts)
		return 0;

	if (mnt_optstr_get_option(opts, name, &suffix, &valsz))
		return 0;

	suffix = strndup(suffix, valsz);
	if (!suffix)
		return -ENOMEM;

	DBG(CXT, ul_debugobj(cxt, "umount: umount.%s %s requested", suffix, name));

	rc = mnt_context_prepare_helper(cxt, "umount", suffix);
	free(suffix);

	return rc;
}