filter_check_mail_from_regex(struct filter *filter, const char *key)
{
	int	ret = 0;

	if (filter->config->mail_from_regex == NULL)
		return 0;

	if (table_match(filter->config->mail_from_regex, K_REGEX, key) > 0)
		ret = 1;
	return filter->config->not_mail_from_regex < 0 ? !ret : ret;
}