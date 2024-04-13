filter_check_fcrdns(struct filter *filter, int fcrdns)
{
	int	ret = 0;

	if (!filter->config->fcrdns)
		return 0;

	ret = fcrdns == 1;
	return filter->config->not_fcrdns < 0 ? !ret : ret;
}