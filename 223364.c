void reset_output_levels(void)
{
	int j;

	memset(info_levels, 0, sizeof info_levels);
	memset(debug_levels, 0, sizeof debug_levels);

	for (j = 0; j < COUNT_INFO; j++)
		info_words[j].priority = DEFAULT_PRIORITY;

	for (j = 0; j < COUNT_DEBUG; j++)
		debug_words[j].priority = DEFAULT_PRIORITY;
}