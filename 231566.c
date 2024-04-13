static int grep_cache(struct grep_opt *opt, const char **paths, int cached)
{
	int hit = 0;
	int nr;
	read_cache();

#if !NO_EXTERNAL_GREP
	/*
	 * Use the external "grep" command for the case where
	 * we grep through the checked-out files. It tends to
	 * be a lot more optimized
	 */
	if (!cached) {
		hit = external_grep(opt, paths, cached);
		if (hit >= 0)
			return hit;
	}
#endif

	for (nr = 0; nr < active_nr; nr++) {
		struct cache_entry *ce = active_cache[nr];
		if (!S_ISREG(ce->ce_mode))
			continue;
		if (!pathspec_matches(paths, ce->name))
			continue;
		if (cached) {
			if (ce_stage(ce))
				continue;
			hit |= grep_sha1(opt, ce->sha1, ce->name, 0);
		}
		else
			hit |= grep_file(opt, ce->name);
		if (ce_stage(ce)) {
			do {
				nr++;
			} while (nr < active_nr &&
				 !strcmp(ce->name, active_cache[nr]->name));
			nr--; /* compensate for loop control */
		}
	}
	free_grep_patterns(opt);
	return hit;
}