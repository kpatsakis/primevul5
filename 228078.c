static int is_associated_fs(const char *devname, struct libmnt_fs *fs)
{
	uintmax_t offset = 0;
	const char *src, *optstr;
	char *val;
	size_t valsz;
	int flags = 0;

	/* check if it begins with /dev/loop */
	if (strncmp(devname, _PATH_DEV_LOOP, sizeof(_PATH_DEV_LOOP) - 1) != 0)
		return 0;

	src = mnt_fs_get_srcpath(fs);
	if (!src)
		return 0;

	/* check for the offset option in @fs */
	optstr = mnt_fs_get_user_options(fs);

	if (optstr &&
	    mnt_optstr_get_option(optstr, "offset", &val, &valsz) == 0) {
		flags |= LOOPDEV_FL_OFFSET;

		if (mnt_parse_offset(val, valsz, &offset) != 0)
			return 0;
	}

	return loopdev_is_used(devname, src, offset, 0, flags);
}