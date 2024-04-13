int mnt_split_optstr(const char *optstr, char **user, char **vfs,
		     char **fs, int ignore_user, int ignore_vfs)
{
	int rc = 0;
	char *name, *val, *str = (char *) optstr;
	size_t namesz, valsz, chunsz;
	struct libmnt_optmap const *maps[2];
	struct ul_buffer xvfs = UL_INIT_BUFFER,
			 xfs = UL_INIT_BUFFER,
			 xuser = UL_INIT_BUFFER;

	if (!optstr)
		return -EINVAL;

	maps[0] = mnt_get_builtin_optmap(MNT_LINUX_MAP);
	maps[1] = mnt_get_builtin_optmap(MNT_USERSPACE_MAP);

	chunsz = strlen(optstr) / 2;

	while (!mnt_optstr_next_option(&str, &name, &namesz, &val, &valsz)) {
		struct ul_buffer *buf = NULL;
		const struct libmnt_optmap *ent = NULL;
		const struct libmnt_optmap *m =
			 mnt_optmap_get_entry(maps, 2, name, namesz, &ent);

		if (ent && !ent->id)
			continue;	/* ignore undefined options (comments) */

		/* ignore name=<value> if options map expects <name> only */
		if (valsz && mnt_optmap_entry_novalue(ent))
			m = NULL;

		if (ent && m && m == maps[0] && vfs) {
			if (ignore_vfs && (ent->mask & ignore_vfs))
				continue;
			if (vfs)
				buf = &xvfs;
		} else if (ent && m && m == maps[1] && user) {
			if (ignore_user && (ent->mask & ignore_user))
				continue;
			if (user)
				buf = &xuser;
		} else if (!m && fs) {
			if (fs)
				buf = &xfs;
		}

		if (buf) {
			if (ul_buffer_is_empty(buf))
				ul_buffer_set_chunksize(buf, chunsz);
			rc = __buffer_append_option(buf, name, namesz, val, valsz);
		}
		if (rc)
			break;
	}

	if (vfs)
		*vfs  = rc ? NULL : ul_buffer_get_data(&xvfs, NULL, NULL);
	if (fs)
		*fs   = rc ? NULL : ul_buffer_get_data(&xfs, NULL, NULL);
	if (user)
		*user = rc ? NULL : ul_buffer_get_data(&xuser, NULL, NULL);
	if (rc) {
		ul_buffer_free_data(&xvfs);
		ul_buffer_free_data(&xfs);
		ul_buffer_free_data(&xuser);
	}

	return rc;
}