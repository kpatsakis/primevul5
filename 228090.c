int mnt_optstr_get_flags(const char *optstr, unsigned long *flags,
		const struct libmnt_optmap *map)
{
	struct libmnt_optmap const *maps[2];
	char *name, *str = (char *) optstr;
	size_t namesz = 0, valsz = 0;
	int nmaps = 0;

	if (!optstr || !flags || !map)
		return -EINVAL;

	maps[nmaps++] = map;

	if (map == mnt_get_builtin_optmap(MNT_LINUX_MAP))
		/*
		 * Add userspace map -- the "user" is interpreted as
		 *                      MS_NO{EXEC,SUID,DEV}.
		 */
		maps[nmaps++] = mnt_get_builtin_optmap(MNT_USERSPACE_MAP);

	while(!mnt_optstr_next_option(&str, &name, &namesz, NULL, &valsz)) {
		const struct libmnt_optmap *ent;
		const struct libmnt_optmap *m;

		m = mnt_optmap_get_entry(maps, nmaps, name, namesz, &ent);
		if (!m || !ent || !ent->id)
			continue;

		/* ignore name=<value> if options map expects <name> only */
		if (valsz && mnt_optmap_entry_novalue(ent))
			continue;

		if (m == map) {				/* requested map */
			if (ent->mask & MNT_INVERT)
				*flags &= ~ent->id;
			else
				*flags |= ent->id;

		} else if (nmaps == 2 && m == maps[1] && valsz == 0) {
			/*
			 * Special case -- translate "user" (but no user=) to
			 * MS_ options
			 */
			if (ent->mask & MNT_INVERT)
				continue;
			if (ent->id & (MNT_MS_OWNER | MNT_MS_GROUP))
				*flags |= MS_OWNERSECURE;
			else if (ent->id & (MNT_MS_USER | MNT_MS_USERS))
				*flags |= MS_SECURE;
		}
	}

	return 0;
}