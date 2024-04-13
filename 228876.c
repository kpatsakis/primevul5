filter_check_rcpt_to_table(struct filter *filter, enum table_service kind, const char *key)
{
	int	ret = 0;

	if (filter->config->rcpt_to_table == NULL)
		return 0;

	if (table_match(filter->config->rcpt_to_table, kind, key) > 0)
		ret = 1;
	return filter->config->not_rcpt_to_table < 0 ? !ret : ret;
}