int rtnl_dump_filter_nc(struct rtnl_handle *rth,
		     rtnl_filter_t filter,
		     void *arg1, __u16 nc_flags)
{
	const struct rtnl_dump_filter_arg a[2] = {
		{ .filter = filter, .arg1 = arg1, .nc_flags = nc_flags, },
		{ .filter = NULL,   .arg1 = NULL, .nc_flags = 0, },
	};

	return rtnl_dump_filter_l(rth, a);
}