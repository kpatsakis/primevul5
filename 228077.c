int mnt_optstr_apply_flags(char **optstr, unsigned long flags,
				const struct libmnt_optmap *map)
{
	struct libmnt_optmap const *maps[1];
	char *name, *next, *val;
	size_t namesz = 0, valsz = 0, multi = 0;
	unsigned long fl;
	int rc = 0;

	if (!optstr || !map)
		return -EINVAL;

	DBG(CXT, ul_debug("applying 0x%08lx flags to '%s'", flags, *optstr));

	maps[0] = map;
	next = *optstr;
	fl = flags;

	/*
	 * There is a convention that 'rw/ro' flags are always at the beginning of
	 * the string (although the 'rw' is unnecessary).
	 */
	if (map == mnt_get_builtin_optmap(MNT_LINUX_MAP)) {
		const char *o = (fl & MS_RDONLY) ? "ro" : "rw";

		if (next &&
		    (!strncmp(next, "rw", 2) || !strncmp(next, "ro", 2)) &&
		    (*(next + 2) == '\0' || *(next + 2) == ',')) {

			/* already set, be paranoid and fix it */
			memcpy(next, o, 2);
		} else {
			rc = mnt_optstr_prepend_option(optstr, o, NULL);
			if (rc)
				goto err;
			next = *optstr;		/* because realloc() */
		}
		fl &= ~MS_RDONLY;
		next += 2;
		if (*next == ',')
			next++;
	}

	if (next && *next) {
		/*
		 * scan @optstr and remove options that are missing in
		 * @flags
		 */
		while(!mnt_optstr_next_option(&next, &name, &namesz,
							&val, &valsz)) {
			const struct libmnt_optmap *ent;

			if (mnt_optmap_get_entry(maps, 1, name, namesz, &ent)) {
				/*
				 * remove unwanted option (rw/ro is already set)
				 */
				if (!ent || !ent->id)
					continue;
				/* ignore name=<value> if options map expects <name> only */
				if (valsz && mnt_optmap_entry_novalue(ent))
					continue;

				if (ent->id == MS_RDONLY ||
				    (ent->mask & MNT_INVERT) ||
				    (fl & ent->id) != (unsigned long) ent->id) {

					char *end = val ? val + valsz :
							  name + namesz;
					next = name;
					rc = mnt_optstr_remove_option_at(
							optstr, name, end);
					if (rc)
						goto err;
				}
				if (!(ent->mask & MNT_INVERT)) {
					/* allow options with prefix (X-mount.foo,X-mount.bar) more than once */
					if (ent->mask & MNT_PREFIX)
						multi |= ent->id;
					else
						fl &= ~ent->id;
					if (ent->id & MS_REC)
						fl |= MS_REC;
				}
			}
		}
	}

	/* remove from flags options which are allowed more than once */
	fl &= ~multi;

	/* add missing options (but ignore fl if contains MS_REC only) */
	if (fl && fl != MS_REC) {

		const struct libmnt_optmap *ent;
		struct ul_buffer buf = UL_INIT_BUFFER;
		size_t sz;
		char *p;

		ul_buffer_refer_string(&buf, *optstr);

		for (ent = map; ent && ent->name; ent++) {
			if ((ent->mask & MNT_INVERT)
			    || ent->id == 0
			    || (fl & ent->id) != (unsigned long) ent->id)
				continue;

			/* don't add options which require values (e.g. offset=%d) */
			p = strchr(ent->name, '=');
			if (p) {
				if (p > ent->name && *(p - 1) == '[')
					p--;			/* name[=] */
				else
					continue;		/* name= */
				sz = p - ent->name;
			} else
				sz = strlen(ent->name);

			rc = __buffer_append_option(&buf, ent->name, sz, NULL, 0);
			if (rc)
				goto err;
		}

		*optstr = ul_buffer_get_data(&buf, NULL, NULL);
	}

	DBG(CXT, ul_debug("new optstr '%s'", *optstr));
	return rc;
err:
	DBG(CXT, ul_debug("failed to apply flags [rc=%d]", rc));
	return rc;
}