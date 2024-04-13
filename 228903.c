filter_check_auth_regex(struct filter *filter, const char *key)
{
	int	ret = 0;

	if (filter->config->auth_regex == NULL)
		return 0;

	if (key && table_match(filter->config->auth_regex, K_REGEX, key) > 0)
		ret = 1;
	return filter->config->not_auth_regex < 0 ? !ret : ret;
}