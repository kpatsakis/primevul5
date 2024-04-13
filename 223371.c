static void set_output_verbosity(int level, uchar priority)
{
	int j;

	if (level > MAX_VERBOSITY)
		level = MAX_VERBOSITY;

	for (j = 1; j <= level; j++) {
		parse_output_words(info_words, info_levels, info_verbosity[j], priority);
		parse_output_words(debug_words, debug_levels, debug_verbosity[j], priority);
	}
}