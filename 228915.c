filter_check_mail_from_table(struct filter *filter, enum table_service kind, const char *key)
{
	int	ret = 0;

	if (filter->config->mail_from_table == NULL)
		return 0;

	if (table_match(filter->config->mail_from_table, kind, key) > 0)
		ret = 1;
	return filter->config->not_mail_from_table < 0 ? !ret : ret;
}