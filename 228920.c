filter_check_helo_regex(struct filter *filter, const char *key)
{
	int	ret = 0;

	if (filter->config->helo_regex == NULL)
		return 0;

	if (table_match(filter->config->helo_regex, K_REGEX, key) > 0)
		ret = 1;
	return filter->config->not_helo_regex < 0 ? !ret : ret;
}