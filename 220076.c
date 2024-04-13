int rtnl_dump_request(struct rtnl_handle *rth, int type, void *req, int len)
{
	struct nlmsghdr nlh = {
		.nlmsg_len = NLMSG_LENGTH(len),
		.nlmsg_type = type,
		.nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST,
		.nlmsg_seq = rth->dump = ++rth->seq,
	};
	struct sockaddr_nl nladdr = { .nl_family = AF_NETLINK };
	struct iovec iov[2] = {
		{ .iov_base = &nlh, .iov_len = sizeof(nlh) },
		{ .iov_base = req, .iov_len = len }
	};
	struct msghdr msg = {
		.msg_name = &nladdr,
		.msg_namelen = sizeof(nladdr),
		.msg_iov = iov,
		.msg_iovlen = 2,
	};

	return sendmsg(rth->fd, &msg, 0);
}