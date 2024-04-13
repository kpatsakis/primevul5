filter_check_rdns_regex(struct filter *filter, const char *key)
{
	int	ret = 0;

	if (filter->config->rdns_regex == NULL)
		return 0;

	if (table_match(filter->config->rdns_regex, K_REGEX, key) > 0)
		ret = 1;
	return filter->config->not_rdns_regex < 0 ? !ret : ret;
}