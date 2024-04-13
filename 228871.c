filter_check_auth_table(struct filter *filter, enum table_service kind, const char *key)
{
	int	ret = 0;

	if (filter->config->auth_table == NULL)
		return 0;
	
	if (key && table_match(filter->config->auth_table, kind, key) > 0)
		ret = 1;

	return filter->config->not_auth_table < 0 ? !ret : ret;
}