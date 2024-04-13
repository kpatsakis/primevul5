int mnt_optstr_get_options(const char *optstr, char **subset,
			    const struct libmnt_optmap *map, int ignore)
{
	struct libmnt_optmap const *maps[1];
	struct ul_buffer buf = UL_INIT_BUFFER;
	char *name, *val, *str = (char *) optstr;
	size_t namesz, valsz;
	int rc = 0;

	if (!optstr || !subset)
		return -EINVAL;

	maps[0] = map;

	ul_buffer_set_chunksize(&buf, strlen(optstr)/2);

	while (!mnt_optstr_next_option(&str, &name, &namesz, &val, &valsz)) {
		const struct libmnt_optmap *ent;

		mnt_optmap_get_entry(maps, 1, name, namesz, &ent);

		if (!ent || !ent->id)
			continue;	/* ignore undefined options (comments) */

		if (ignore && (ent->mask & ignore))
			continue;

		/* ignore name=<value> if options map expects <name> only */
		if (valsz && mnt_optmap_entry_novalue(ent))
			continue;

		rc = __buffer_append_option(&buf, name, namesz, val, valsz);
		if (rc)
			break;
	}

	*subset  = rc ? NULL : ul_buffer_get_data(&buf, NULL, NULL);
	if (rc)
		ul_buffer_free_data(&buf);
	return rc;
}