int rtnl_send(struct rtnl_handle *rth, const void *buf, int len)
{
	return send(rth->fd, buf, len, 0);
}