int rtnl_listen_all_nsid(struct rtnl_handle *rth)
{
	unsigned int on = 1;

	if (setsockopt(rth->fd, SOL_NETLINK, NETLINK_LISTEN_ALL_NSID, &on,
		       sizeof(on)) < 0) {
		perror("NETLINK_LISTEN_ALL_NSID");
		return -1;
	}
	rth->flags |= RTNL_HANDLE_F_LISTEN_ALL_NSID;
	return 0;
}