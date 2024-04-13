filter_check_rdns(struct filter *filter, const char *hostname)
{
	int	ret = 0;
	struct netaddr	netaddr;

	if (!filter->config->rdns)
		return 0;

	/* this is a hack until smtp session properly deals with lack of rdns */
	ret = strcmp("<unknown>", hostname);
	if (ret == 0)
		return filter->config->not_rdns < 0 ? !ret : ret;

	/* if text_to_netaddress succeeds,
	 * we don't have an rDNS so the filter should match
	 */
	ret = !text_to_netaddr(&netaddr, hostname);
	return filter->config->not_rdns < 0 ? !ret : ret;
}