filter_check_auth(struct filter *filter, const char *username)
{
	int ret = 0;

	if (!filter->config->auth)
		return 0;

	ret = username ? 1 : 0;

	return filter->config->not_auth < 0 ? !ret : ret;
}