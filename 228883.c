filter_check_src_regex(struct filter *filter, const char *key)
{
	int	ret = 0;

	if (filter->config->src_regex == NULL)
		return 0;

	if (table_match(filter->config->src_regex, K_REGEX, key) > 0)
		ret = 1;
	return filter->config->not_src_regex < 0 ? !ret : ret;
}