void limit_output_verbosity(int level)
{
	short info_limits[COUNT_INFO], debug_limits[COUNT_DEBUG];
	int j;

	if (level > MAX_VERBOSITY)
		return;

	memset(info_limits, 0, sizeof info_limits);
	memset(debug_limits, 0, sizeof debug_limits);

	/* Compute the level limits in the above arrays. */
	for (j = 1; j <= level; j++) {
		parse_output_words(info_words, info_limits, info_verbosity[j], LIMIT_PRIORITY);
		parse_output_words(debug_words, debug_limits, debug_verbosity[j], LIMIT_PRIORITY);
	}

	for (j = 0; j < COUNT_INFO; j++) {
		if (info_levels[j] > info_limits[j])
			info_levels[j] = info_limits[j];
	}

	for (j = 0; j < COUNT_DEBUG; j++) {
		if (debug_levels[j] > debug_limits[j])
			debug_levels[j] = debug_limits[j];
	}
}