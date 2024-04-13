filter_check_rdns_table(struct filter *filter, enum table_service kind, const char *key)
{
	int	ret = 0;

	if (filter->config->rdns_table == NULL)
		return 0;
	
	if (table_match(filter->config->rdns_table, kind, key) > 0)
		ret = 1;

	return filter->config->not_rdns_table < 0 ? !ret : ret;
}