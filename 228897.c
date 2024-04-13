filter_check_helo_table(struct filter *filter, enum table_service kind, const char *key)
{
	int	ret = 0;

	if (filter->config->helo_table == NULL)
		return 0;

	if (table_match(filter->config->helo_table, kind, key) > 0)
		ret = 1;
	return filter->config->not_helo_table < 0 ? !ret : ret;
}