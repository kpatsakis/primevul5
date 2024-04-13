filter_check_rcpt_to_regex(struct filter *filter, const char *key)
{
	int	ret = 0;

	if (filter->config->rcpt_to_regex == NULL)
		return 0;

	if (table_match(filter->config->rcpt_to_regex, K_REGEX, key) > 0)
		ret = 1;
	return filter->config->not_rcpt_to_regex < 0 ? !ret : ret;
}