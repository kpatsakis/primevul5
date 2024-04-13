void negate_output_levels(void)
{
	int j;

	for (j = 0; j < COUNT_INFO; j++)
		info_levels[j] *= -1;

	for (j = 0; j < COUNT_DEBUG; j++)
		debug_levels[j] *= -1;
}