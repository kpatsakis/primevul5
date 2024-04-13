int rtnl_wilddump_request(struct rtnl_handle *rth, int family, int type)
{
	return rtnl_wilddump_req_filter(rth, family, type, RTEXT_FILTER_VF);
}