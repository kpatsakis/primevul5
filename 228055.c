int mnt_optstr_remove_option(char **optstr, const char *name)
{
	struct libmnt_optloc ol = MNT_INIT_OPTLOC;
	int rc;

	if (!optstr || !name)
		return -EINVAL;

	rc = mnt_optstr_locate_option(*optstr, name, &ol);
	if (rc != 0)
		return rc;

	mnt_optstr_remove_option_at(optstr, ol.begin, ol.end);
	return 0;
}