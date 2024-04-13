static int has_utab_entry(struct libmnt_context *cxt, const char *target)
{
	struct libmnt_cache *cache = NULL;
	struct libmnt_fs *fs;
	struct libmnt_iter itr;
	char *cn = NULL;
	int rc = 0;

	assert(cxt);

	if (!cxt->utab) {
		const char *path = mnt_get_utab_path();

		if (!path || is_file_empty(path))
			return 0;
		cxt->utab = mnt_new_table();
		if (!cxt->utab)
			return 0;
		cxt->utab->fmt = MNT_FMT_UTAB;
		if (mnt_table_parse_file(cxt->utab, path))
			return 0;
	}

	/* paths in utab are canonicalized */
	cache = mnt_context_get_cache(cxt);
	cn = mnt_resolve_path(target, cache);
	mnt_reset_iter(&itr, MNT_ITER_BACKWARD);

	while (mnt_table_next_fs(cxt->utab, &itr, &fs) == 0) {
		if (mnt_fs_streq_target(fs, cn)) {
			rc = 1;
			break;
		}
	}

	if (!cache)
		free(cn);
	return rc;
}