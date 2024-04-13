filter_check_src_table(struct filter *filter, enum table_service kind, const char *key)
{
	int	ret = 0;

	if (filter->config->src_table == NULL)
		return 0;

	if (table_match(filter->config->src_table, kind, key) > 0)
		ret = 1;
	return filter->config->not_src_table < 0 ? !ret : ret;
}